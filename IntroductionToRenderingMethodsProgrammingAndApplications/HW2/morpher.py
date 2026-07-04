import numpy as np
import cv2
from models import FeatureLine

try:
    import cupy as cp
    # Try a simple allocation to see if CUDA is actually working
    cp.zeros(1)
    HAS_CUPY = True
except (ImportError, Exception):
    import numpy as cp
    HAS_CUPY = False

class MorphEngine:
    """
    Stateless Morphing Engine implementing the Beier-Neely algorithm.
    Features auto-detection for CuPy (GPU acceleration).
    """
    CONST_A = 0.001
    CONST_B = 2.0
    CONST_P = 0.5

    @staticmethod
    def process(imgA, imgB, alpha: float) -> tuple:
        """
        Main public interface. alpha is from 0.0 to 1.0.
        Returns a tuple (Warped_A, Warped_B) as numpy ndarrays.
        """
        linesA = imgA.get_lines()
        linesB = imgB.get_lines()
        
        # Interpolate lines for current alpha
        dest_lines = MorphEngine._interpolate_lines(linesA, linesB, alpha)
        
        # Warp both images
        warpedA = MorphEngine._compute_warp(imgA.image, linesA, dest_lines)
        warpedB = MorphEngine._compute_warp(imgB.image, linesB, dest_lines)
        
        # Ensure they are numpy arrays
        if HAS_CUPY:
            if hasattr(warpedA, 'get'): warpedA = warpedA.get()
            if hasattr(warpedB, 'get'): warpedB = warpedB.get()
            
        return warpedA, warpedB

    @staticmethod
    def _interpolate_lines(linesA: list, linesB: list, t: float) -> list:
        """
        Linearly interpolate endpoints of two sets of feature lines.
        """
        interpolated = []
        for la, lb in zip(linesA, linesB):
            start = (
                la.start_point[0] * (1 - t) + lb.start_point[0] * t,
                la.start_point[1] * (1 - t) + lb.start_point[1] * t
            )
            end = (
                la.end_point[0] * (1 - t) + lb.end_point[0] * t,
                la.end_point[1] * (1 - t) + lb.end_point[1] * t
            )
            interpolated.append(FeatureLine(start, end))
        return interpolated

    @staticmethod
    def _compute_warp(image: np.ndarray, src_lines: list, dest_lines: list) -> np.ndarray:
        """
        Core Beier-Neely algorithm using Reverse Mapping.
        """
        height, width = image.shape[:2]
        
        # 1. Create coordinate grid (x, y)
        x, y = cp.meshgrid(cp.arange(width), cp.arange(height))
        X = cp.stack((x, y), axis=-1).astype(cp.float32) # (H, W, 2)
        
        # Use CuPy version of the image if available
        image_cp = cp.array(image) if HAS_CUPY else image
        
        # DSUM: cumulative displacements, weightsum: cumulative weights
        DSUM = cp.zeros_like(X)
        weightsum = cp.zeros((height, width), dtype=cp.float32)
        
        for sl, dl in zip(src_lines, dest_lines):
            # Points P and Q from destination line segment
            P = cp.array(dl.start_point, dtype=cp.float32)
            Q = cp.array(dl.end_point, dtype=cp.float32)
            
            # Points P' and Q' from source line segment
            P_prime = cp.array(sl.start_point, dtype=cp.float32)
            Q_prime = cp.array(sl.end_point, dtype=cp.float32)
            
            # Vector QP = Q - P
            QP = Q - P
            len_sq_QP = cp.sum(QP**2)
            len_QP = cp.sqrt(len_sq_QP)
            
            # Perpendicular vector: (-y, x)
            perp_QP = cp.array([-QP[1], QP[0]])
            
            # u = (X - P) . (Q - P) / ||Q - P||^2
            X_minus_P = X - P
            u = (X_minus_P[..., 0] * QP[0] + X_minus_P[..., 1] * QP[1]) / (len_sq_QP + 1e-6)
            
            # v = (X - P) . Perp(Q - P) / ||Q - P||
            v = (X_minus_P[..., 0] * perp_QP[0] + X_minus_P[..., 1] * perp_QP[1]) / (len_QP + 1e-6)
            
            # Source coordinates X_prime
            QP_prime = Q_prime - P_prime
            len_sq_QP_prime = cp.sum(QP_prime**2)
            len_QP_prime = cp.sqrt(len_sq_QP_prime)
            perp_QP_prime = cp.array([-QP_prime[1], QP_prime[0]])
            
            X_prime = P_prime + u[..., cp.newaxis] * QP_prime + v[..., cp.newaxis] * perp_QP_prime / (len_QP_prime + 1e-6)
            
            # Displacement Di = Xi' - Xi
            Di = X_prime - X
            
            # Distance from point to line segment
            dist = cp.where(u < 0, 
                            cp.sqrt(cp.sum(X_minus_P**2, axis=-1)),
                            cp.where(u > 1,
                                     cp.sqrt(cp.sum((X - Q)**2, axis=-1)),
                                     cp.abs(v)))
            
            # Weight = (length^p / (a + dist))^b
            weight = (len_QP**MorphEngine.CONST_P / (MorphEngine.CONST_A + dist))**MorphEngine.CONST_B
            
            DSUM += Di * weight[..., cp.newaxis]
            weightsum += weight
            
        # Final source coordinates: X' = X + DSUM / weightsum
        X_prime_final = X + DSUM / (weightsum[..., cp.newaxis] + 1e-6)
        
        # Transfer back to CPU for cv2.remap if using CuPy
        if HAS_CUPY:
            map_x = X_prime_final[..., 0].get()
            map_y = X_prime_final[..., 1].get()
            src_img = image # image is already numpy
        else:
            map_x = X_prime_final[..., 0]
            map_y = X_prime_final[..., 1]
            src_img = image
        
        warped_img = cv2.remap(src_img, map_x, map_y, 
                               interpolation=cv2.INTER_LINEAR, 
                               borderMode=cv2.BORDER_REPLICATE)
        return warped_img

import cv2
import numpy as np
from models import FeatureLine, MorphImage

class ImageEditor:
    """
    OpenCV Canvas Controller for drawing feature lines on a MorphImage.
    """
    def __init__(self, window_name, morph_img: MorphImage):
        self.window_name = window_name
        self.morph_img = morph_img
        self.is_drawing = False
        self.start_point = None
        self.temp_end_point = None
        
        # Color palette for lines
        self.palette = [
            (255, 0, 0),   # Blue
            (0, 255, 0),   # Green
            (0, 0, 255),   # Red
            (0, 255, 255), # Cyan
            (255, 0, 255), # Magenta
            (255, 255, 0)  # Yellow
        ]

    def _mouse_callback(self, event, x, y, flags, param):
        """
        Handle mouse events for line drawing.
        """
        if event == cv2.EVENT_LBUTTONDOWN:
            self.is_drawing = True
            self.start_point = (x, y)
            self.temp_end_point = (x, y)
            
        elif event == cv2.EVENT_MOUSEMOVE:
            if self.is_drawing:
                self.temp_end_point = (x, y)
                self._draw_overlay()
                
        elif event == cv2.EVENT_LBUTTONUP:
            if self.is_drawing:
                self.is_drawing = False
                end_point = (x, y)
                # Add line only if it has length
                if self.start_point != end_point:
                    new_line = FeatureLine(self.start_point, end_point)
                    self.morph_img.add_line(new_line)
                self.start_point = None
                self.temp_end_point = None
                self._draw_overlay()

    def _draw_overlay(self):
        """
        Redraw the image with existing lines and the current temporary line.
        """
        # Create a copy of the base image
        canvas = self.morph_img.image.copy()
        
        # Draw existing lines
        lines = self.morph_img.get_lines()
        for idx, line in enumerate(lines):
            color = self.palette[idx % len(self.palette)]
            # Draw as arrow (start to end)
            cv2.arrowedLine(canvas, line.start_point, line.end_point, color, 2, tipLength=0.1)
            # Label the start point with index
            cv2.putText(canvas, str(idx), line.start_point, 
                        cv2.FONT_HERSHEY_SIMPLEX, 0.8, color, 2)

        # Draw temporary line if drawing
        if self.is_drawing and self.start_point and self.temp_end_point:
            color = self.palette[len(lines) % len(self.palette)]
            # Draw as arrow for preview
            cv2.arrowedLine(canvas, self.start_point, self.temp_end_point, color, 1, cv2.LINE_AA, tipLength=0.1)

        cv2.imshow(self.window_name, canvas)

    def show(self):
        """
        Initialize the window and wait for interactions.
        """
        cv2.namedWindow(self.window_name)
        cv2.setMouseCallback(self.window_name, self._mouse_callback)
        self._draw_overlay()
        print(f"Opening editor: {self.window_name}. Press 'Enter' or close window to finish.")
        
        while True:
            key = cv2.waitKey(20) & 0xFF
            # Press 'Enter' (13) or Esc (27) to finish
            if key == 13 or key == 27:
                break
            # Check if window was closed (X button)
            if cv2.getWindowProperty(self.window_name, cv2.WND_PROP_VISIBLE) < 1:
                break
        
        try:
            cv2.destroyWindow(self.window_name)
        except cv2.error:
            pass

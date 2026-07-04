import tkinter as tk
from tkinter import filedialog, ttk
from tkinter import messagebox
import cv2
import numpy as np
import threading
import queue
import time
import os
from PIL import Image, ImageTk
from models import MorphImage, FeatureLine
from editor import ImageEditor
from morpher import MorphEngine
import pickle

class MainWindow:
    """
    Tkinter Main Window and Thread Manager.
    Handles UI, multithreading, and ROI blending.
    """
    def __init__(self, root):
        self.root = root
        self.root.title("ImageMorpher - Beier-Neely Morphing")
        
        # Data
        self.morph_images = [None, None, None] # Img1, Img2, Img3
        
        # Threading
        self.input_queue = queue.Queue()
        self.output_queue = queue.Queue()
        self.worker_thread = threading.Thread(target=self.worker_loop, daemon=True)
        self.worker_thread.start()
        
        self.setup_ui()
        self.update_ui_loop()

    def setup_ui(self):
        # Top Control Panel
        control_frame = tk.Frame(self.root)
        control_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=10)
        
        tk.Button(control_frame, text="Load Image 1", command=lambda: self.load_image(0)).pack(side=tk.LEFT, padx=5)
        tk.Button(control_frame, text="Load Image 2", command=lambda: self.load_image(1)).pack(side=tk.LEFT, padx=5)
        tk.Button(control_frame, text="Load Image 3", command=lambda: self.load_image(2)).pack(side=tk.LEFT, padx=5)
        tk.Button(control_frame, text="Save State (.pkl)", command=self.save_state).pack(side=tk.LEFT, padx=5)

        # Alpha Slider
        alpha_frame = tk.LabelFrame(self.root, text="Morphing Control (Alpha)")
        alpha_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=5)
        self.alpha_slider = tk.Scale(alpha_frame, from_=0.0, to=2.0, resolution=0.01, 
                                     orient=tk.HORIZONTAL, command=self.on_slider_changed)
        self.alpha_slider.set(0.0)
        self.alpha_slider.pack(fill=tk.X, padx=5, pady=5)

        # ROI Control
        roi_frame = tk.LabelFrame(self.root, text="ROI Local Morphing")
        roi_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=5)
        
        self.roi_enabled = tk.BooleanVar(value=False)
        tk.Checkbutton(roi_frame, text="Enable ROI", variable=self.roi_enabled, 
                       command=self.on_slider_changed).pack(side=tk.LEFT, padx=5)
        
        # ROI X1, Y1, X2, Y2 (as percentage 0-100 for now, or resolution-based)
        self.roi_x1 = tk.Scale(roi_frame, from_=0, to=100, orient=tk.HORIZONTAL, label="X1%", command=self.on_slider_changed)
        self.roi_y1 = tk.Scale(roi_frame, from_=0, to=100, orient=tk.HORIZONTAL, label="Y1%", command=self.on_slider_changed)
        self.roi_x2 = tk.Scale(roi_frame, from_=0, to=100, orient=tk.HORIZONTAL, label="X2%", command=self.on_slider_changed)
        self.roi_y2 = tk.Scale(roi_frame, from_=0, to=100, orient=tk.HORIZONTAL, label="Y2%", command=self.on_slider_changed)
        self.roi_x1.pack(side=tk.LEFT, padx=5); self.roi_y1.pack(side=tk.LEFT, padx=5)
        self.roi_x2.pack(side=tk.LEFT, padx=5); self.roi_y2.pack(side=tk.LEFT, padx=5)
        self.roi_x2.set(100); self.roi_y2.set(100)

        # 2x2 Grid Display
        self.grid_frame = tk.Frame(self.root)
        self.grid_frame.pack(expand=True, fill=tk.BOTH, padx=10, pady=10)
        
        self.slots = []
        for i in range(4):
            lbl = tk.Label(self.grid_frame, text=f"Slot {i+1}", borderwidth=2, relief="groove", width=40, height=20)
            lbl.grid(row=i//2, column=i%2, sticky="nsew")
            self.slots.append(lbl)
            self.grid_frame.grid_columnconfigure(i%2, weight=1)
            self.grid_frame.grid_rowconfigure(i//2, weight=1)

    def load_image(self, index):
        # Requirement: If slot already has an image, open editor directly.
        if self.morph_images[index] is not None:
            mimg = self.morph_images[index]
            editor = ImageEditor(f"Marking Image {index+1}", mimg)
            editor.show()
            self.on_slider_changed() # Update if lines changed
            return

        path = filedialog.askopenfilename(filetypes=[("Images/Pickle", "*.jpg *.png *.jpeg *.pkl")])
        if not path: return
        
        if path.endswith('.pkl'):
            import pickle
            with open(path, 'rb') as f:
                mimg = pickle.load(f)
        else:
            img = cv2.imread(path)
            if img is None: return
            mimg = MorphImage(img, os.path.basename(path))
            # Open Editor for feature marking
            editor = ImageEditor(f"Marking Image {index+1}", mimg)
            editor.show()
            
        self.morph_images[index] = mimg
        
        # Update ROI slider ranges if this is the first image or resolution changed
        if mimg.image is not None:
            h, w = mimg.image.shape[:2]
            self.roi_x1.config(to=w, label="X1")
            self.roi_x2.config(to=w, label="X2")
            self.roi_y1.config(to=h, label="Y1")
            self.roi_y2.config(to=h, label="Y2")
            if self.roi_x2.get() == 100: self.roi_x2.set(w)
            if self.roi_y2.get() == 100: self.roi_y2.set(h)

        self.on_slider_changed() # Trigger update

    def save_state(self):
        for i in range(len(self.morph_images)):
            if not self.morph_images[i]: 
                messagebox.showwarning("Warning", f"No image in Slot {i+1} to save.")
                continue
                
            mimg = self.morph_images[i]
            folder = filedialog.askdirectory(title="Select Target Folder to Save .pkl")
            if not folder: continue
            
            # Default filename: original_filename + .pkl
            base_name = os.path.splitext(mimg.original_filename)[0]
            save_path = os.path.join(folder, base_name + ".pkl")
            
            try:
                with open(save_path, 'wb') as f:
                    pickle.dump(mimg, f)
                messagebox.showinfo("Success", f"Saved state to {save_path}")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to save: {e}")

    def on_slider_changed(self, *args):
        # Pack current parameters into input_queue
        params = {
            'alpha': self.alpha_slider.get(),
            'roi_enabled': self.roi_enabled.get(),
            'roi_rect': (self.roi_x1.get(), self.roi_y1.get(), self.roi_x2.get(), self.roi_y2.get()),
            'images': self.morph_images
        }
        self.input_queue.put(params)

    def worker_loop(self):
        while True:
            # Debouncing: Get the latest task and clear old ones
            task = None
            try:
                while True:
                    task = self.input_queue.get_nowait()
            except queue.Empty:
                pass
            
            if task is None:
                time.sleep(0.01)
                continue
                
            alpha = task['alpha']
            imgs = task['images']
            
            # Identify which images to use
            # 0.0 ~ 1.0 -> Img1 and Img2
            # 1.0 ~ 2.0 -> Img2 and Img3
            
            res_images = [None, None, None, None] # Slot 1, 2, 3, 4
            
            if alpha <= 1.0:
                img_A = imgs[0]; img_B = imgs[1]
                t = alpha
            else:
                img_A = imgs[1]; img_B = imgs[2]
                t = alpha - 1.0

            if img_A and img_B:
                # Core computation
                wA, wB = MorphEngine.process(img_A, img_B, t)
                
                # ROI Masking
                h, w = wA.shape[:2]
                if task['roi_enabled']:
                    x1, y1, x2, y2 = task['roi_rect']
                    # Ensure rect is valid and within bounds
                    ix1, ix2 = max(0, min(x1, w)), max(0, min(x2, w))
                    iy1, iy2 = max(0, min(y1, h)), max(0, min(y2, h))
                    ix1, ix2 = min(ix1, ix2), max(ix1, ix2)
                    iy1, iy2 = min(iy1, iy2), max(iy1, iy2)
                    
                    mask = np.zeros((h, w), dtype=np.float32)
                    if ix2 > ix1 and iy2 > iy1:
                        mask[iy1:iy2, ix1:ix2] = 1.0
                    mask = cv2.GaussianBlur(mask, (21, 21), 0)
                    mask = np.stack([mask]*3, axis=-1)
                    
                    final = (wB * mask + wA * (1 - mask)).astype(np.uint8)
                else:
                    final = (wA * (1 - t) + wB * t).astype(np.uint8)
                
                # Slot assignments
                if alpha <= 1.0:
                    res_images[0] = wA # Slot 1
                    res_images[1] = wB # Slot 2
                    res_images[2] = imgs[2].image if imgs[2] else None # Slot 3
                else:
                    res_images[0] = imgs[0].image if imgs[0] else None # Slot 1
                    res_images[1] = wA # Slot 2 (Img2 warped)
                    res_images[2] = wB # Slot 3 (Img3 warped)
                
                res_images[3] = final # Slot 4
            else:
                # Default display if images missing
                res_images[0] = imgs[0].image if imgs[0] else None
                res_images[1] = imgs[1].image if imgs[1] else None
                res_images[2] = imgs[2].image if imgs[2] else None
                res_images[3] = None

            # Memory safety: copy result
            safe_results = [np.copy(img) if img is not None else None for img in res_images]
            self.output_queue.put(safe_results)

    def update_ui_loop(self):
        try:
            results = None
            while True:
                results = self.output_queue.get_nowait()
        except queue.Empty:
            pass
            
        if results:
            for i, img_arr in enumerate(results):
                if img_arr is not None:
                    # Resize to fit slot roughly (keeping aspect ratio or fixed)
                    h, w = img_arr.shape[:2]
                    # Simple resize for preview
                    preview_size = (300, 200)
                    img_resized = cv2.resize(img_arr, preview_size)
                    
                    img_rgb = cv2.cvtColor(img_resized, cv2.COLOR_BGR2RGB)
                    img_pil = Image.fromarray(img_rgb)
                    img_tk = ImageTk.PhotoImage(image=img_pil)
                    
                    self.slots[i].config(image=img_tk, text="")
                    self.slots[i].image = img_tk # Keep reference
                else:
                    self.slots[i].config(image="", text=f"Slot {i+1}")
        
        self.root.after(30, self.update_ui_loop)

import tkinter as tk
import argparse
import sys
import os
import cv2
import pickle
from gui import MainWindow
from models import MorphImage

def load_to_slot(app, slot_index, path):
    if not os.path.exists(path):
        print(f"File not found for slot {slot_index+1}: {path}")
        return
        
    if path.endswith('.pkl'):
        try:
            with open(path, 'rb') as f:
                mimg = pickle.load(f)
            app.morph_images[slot_index] = mimg
        except Exception as e:
            print(f"Failed to load .pkl into slot {slot_index+1}: {e}")
    else:
        img = cv2.imread(path)
        if img is not None:
            # Pass original_filename as required
            app.morph_images[slot_index] = MorphImage(img, os.path.basename(path))
        else:
            print(f"Failed to read image into slot {slot_index+1}: {path}")

def main():
    parser = argparse.ArgumentParser(description="ImageMorpher - Beier-Neely Morphing Tool")
    parser.add_argument("--slot1", help="Image or .pkl for Image 1")
    parser.add_argument("--slot2", help="Image or .pkl for Image 2")
    parser.add_argument("--slot3", help="Image or .pkl for Image 3")
    parser.add_argument("file", nargs="?", help="Legacy argument: path to load into Image 1")
    args = parser.parse_args()

    root = tk.Tk()
    app = MainWindow(root)

    # Handle --slotX arguments
    if args.slot1: load_to_slot(app, 0, args.slot1)
    if args.slot2: load_to_slot(app, 1, args.slot2)
    if args.slot3: load_to_slot(app, 2, args.slot3)
    
    # Handle legacy 'file' argument only if --slot1 wasn't used
    if args.file and not args.slot1:
        load_to_slot(app, 0, args.file)

    # After loading, update ROI ranges based on the first available image
    for mimg in app.morph_images:
        if mimg and mimg.image is not None:
            h, w = mimg.image.shape[:2]
            app.roi_x1.config(to=w, label="X1")
            app.roi_x2.config(to=w, label="X2")
            app.roi_y1.config(to=h, label="Y1")
            app.roi_y2.config(to=h, label="Y2")
            # Set default ROI to full image if not already set
            if app.roi_x2.get() == 100: app.roi_x2.set(w)
            if app.roi_y2.get() == 100: app.roi_y2.set(h)
            break

    app.on_slider_changed()
    root.mainloop()

if __name__ == "__main__":
    main()

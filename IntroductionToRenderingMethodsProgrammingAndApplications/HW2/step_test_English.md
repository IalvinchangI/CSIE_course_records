# Six-Stage Progressive Development and Testing Plan

### Phase 1: Basic Data Structure Setup (Data Models)
**Goal**: Build the lowest-level immutable data containers, including filename memory, and ensure they support Pickle serialization.

* **Implementation Steps (`models.py`)**:
    1. Implement `FeatureLine`, accepting only `start_point` and `end_point`.
    2. Implement `MorphImage`, initialize by passing `image` (numpy array) and **`original_filename` (str)**, and create management methods like `add_line`, `get_lines`, and `clear_lines`.
* **Corresponding Tests (Unit Test)**:
    * ✅ **T1-1 (Addition Test)**: Instantiate `MorphImage`, call `add_line` three consecutive times, and assert `len(get_lines()) == 3`.
    * ✅ **T1-2 (Serialization and Filename Test)**: Serialize a `MorphImage` containing feature lines and `original_filename='test.jpg'` using `pickle.dumps`, then deserialize with `pickle.loads`. Assert that the unwrapped image matrix values are identical, the feature line coordinates are not lost, and `original_filename` remains `'test.jpg'`.

### Phase 2: Independent Morphing Engine Development (MorphEngine Core)
**Goal**: Verify the correctness of the Beier-Neely core mathematical formulas, boundary protection, and the **CuPy hardware acceleration switch**.

* **Implementation Steps (`morpher.py`)**:
    1. Define class constants $a=0.001$, $b=2.0$, and $p=0.5$.
    2. **Implement Module Detection**: Try `import cupy`. If successful, set an internal flag to enable GPU acceleration; otherwise, fallback to using NumPy.
    3. Implement `_interpolate_lines` (interpolate two sets of feature lines based on $t$).
    4. Implement the core `_compute_warp`: Use meshgrid to generate coordinates, apply vectorized formulas, and finally apply `cv2.remap(..., borderMode=cv2.BORDER_REPLICATE)`.
    5. Implement `process`, connecting the above two methods.
* **Corresponding Tests (Unit/Script Test)**:
    * ✅ **T2-1 (Interpolation Accuracy)**: Input segments $A(0,0)\to(10,10)$ and $B(10,10)\to(20,20)$, set $t=0.5$, and assert the output transition segment is $(5,5)\to(15,15)$.
    * ✅ **T2-2 (Accelerator Switch Test)**: If CuPy is present in the environment, assert that the `process` of the engine still returns a standard `numpy.ndarray` (the engine must internally handle the `.get()` conversion and never leak cupy arrays to the UI).
    * ✅ **T2-3 (Single Line Deformation & Boundary Non-Crash)**: Pass extreme feature lines (that would push pixels off-screen), call `_compute_warp`, and visually confirm with `cv2.imshow` that the square is correctly stretched and the edges are extended (`BORDER_REPLICATE`) without any `IndexError`.

### Phase 3: Interactive Marking Canvas (ImageEditor)
**Goal**: Allow users to smoothly draw lines on the image and provide clear directional visual feedback using "arrows".

* **Implementation Steps (`editor.py`)**:
    1. Create `ImageEditor`, bound to a `MorphImage`.
    2. Implement `_mouse_callback` to handle click, drag, and release events.
    3. Implement `_draw_overlay`: **Use `cv2.arrowedLine` (instead of a regular line)**, dynamically calculate color `color = palette[index % len]`, and use `cv2.putText` to label the index.
* **Corresponding Tests (Manual UI Test)**:
    * ✅ **T3-1 (Arrow Direction and Index Validation)**: Open the canvas and draw a line from top-left to bottom-right. Confirm there is an "arrow" at the endpoint indicating the direction, and it is labeled with index `0`.
    * ✅ **T3-2 (Real-time Preview Test)**: When the mouse is clicked and dragged, a real-time dashed/solid arrow line following the cursor must be visible on the screen.

### Phase 4: Main Window and Multithreaded Debouncing (GUI & Multithreading)
**Goal**: Build the Tkinter interface and establish the Producer-Consumer thread architecture.

* **Implementation Steps (`gui.py`)**:
    1. Use Tkinter to build the 2x2 Grid, Alpha slider (**strictly constrained to `resolution=0.01`**), ROI sliders, and Checkbox.
    2. Create `input_queue` and `output_queue`.
    3. Implement `on_slider_changed` to pack parameters into the Input Queue.
    4. Implement `worker_loop`'s **LIFO queue-clearing debouncing logic** (temporarily use `time.sleep(0.2)` to fake computation).
    5. Implement `update_ui_loop` for polling updates.
* **Corresponding Tests (Concurrency Test)**:
    * ✅ **T4-1 (Resolution and Debouncing Test)**: Move the Alpha slider and assert its value changes exactly by units of `0.01`. When frantically dragging it back and forth, the window must absolutely not freeze; after dragging stops, the screen should only update to the result of the final parameter, ignoring the accumulated old tasks.

### Phase 5: System Integration and ROI Splicing (Integration & Splicing)
**Goal**: Plug the real `MorphEngine` into the Worker Thread and implement advanced mask feathering and splicing.

* **Implementation Steps (`worker_loop` in `gui.py`)**:
    1. Remove fake computation and connect `MorphEngine.process(img_A, img_B, alpha)`.
    2. Implement ROI logic: Generate an Alpha Mask, use `cv2.GaussianBlur` for feathering, apply `Result = B*Mask + A*(1-Mask)`, and pass a `np.copy` to the UI.
* **Corresponding Tests (Visual/Integration Test)**:
    * ✅ **T5-1 (ROI Feathering Splicing Test)**: Set Alpha to 0.5, turn on ROI, and shrink the range to the center. Assert that the center of Slot 3 is `Warped_B`, the outer area is `Warped_A`, and the boundary is a smooth transition without jaggedness.

### Phase 6: System Persistence and Packaging (I/O & Packaging)
**Goal**: Handle specific slot CLI loading, dynamic loading, folder selection for saving, and generate the executable file.

* **Implementation Steps (`main.py` & `gui.py`)**:
    1. Implement CLI `argparse` in `main.py`: Add parameters like `--slot1`, `--slot2`, `--slot3` to load images or pickle files into their respective slots.
    2. Implement "Load button": If the slot is empty, open `filedialog.askopenfilename`; if an image already exists, directly open `ImageEditor` to allow adding more lines.
    3. Implement "Save button": Open `filedialog.askdirectory` to select a folder, and use `original_filename` + `.pkl` as the default filename to perform `pickle.dump`.
    4. Run `PyInstaller` for packaging.
* **Corresponding Tests (E2E Test)**:
    * ✅ **T6-1 (Dynamic Save and CLI Restore Test)**: Click to load Image 1, draw 3 lines. Click save, select Desktop, and confirm the filename defaults to the original filename `.pkl`. After closing the program, launch it via CLI `python main.py --slot2 Desktop/original_filename.pkl`. Assert that this image appears in the "Image 2" slot and the arrow lines are perfectly intact.
    * ✅ **T6-2 (Cross-Platform Packaging)**: Run the generated executable and confirm no `ModuleNotFoundError` is thrown.

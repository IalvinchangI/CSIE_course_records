import numpy as np
import pickle

class FeatureLine:
    """
    Pure Geometric Data for a single feature line.
    """
    def __init__(self, start_point, end_point):
        """
        Initialize with start and end coordinates (x, y).
        """
        self.start_point = tuple(start_point)
        self.end_point = tuple(end_point)

class MorphImage:
    """
    Container for an image and its associated feature lines.
    Supports serialization via pickle.
    """
    def __init__(self, image_array, original_filename: str):
        """
        Initialize with a numpy BGR pixel matrix and the original filename.
        """
        self.image = image_array
        self.original_filename = original_filename
        self.lines = []

    def add_line(self, line: FeatureLine):
        """
        Append a new FeatureLine to the list.
        """
        self.lines.append(line)

    def get_lines(self) -> list:
        """
        Return the list of all feature lines.
        """
        return self.lines

    def clear_lines(self):
        """
        Clear all line segments.
        """
        self.lines = []

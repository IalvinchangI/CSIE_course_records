from scipy.fft import fft
import numpy
from pprint import pprint

# SAMPLE = 25
# RANGE = (0, SAMPLE)
# CUTOFF = 25

x = numpy.array([1, 2, 0, -1, -2, 1, 5, 4])
y = fft(x)

# Output
pprint(y)

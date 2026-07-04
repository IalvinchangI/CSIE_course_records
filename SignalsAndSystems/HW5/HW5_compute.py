import numpy
from numpy import exp, pi
from pprint import pprint

# 6.21 - a
print("6.21 - a")
n = numpy.arange(0, 4)
X = exp(-1j * (pi / 2) * n) + 2 * exp(-1j * (pi) * n) + 3 * exp(-1j * (3 * pi / 2) * n)
pprint(X)

# 6.21 - b
print("6.21 - b")
n = numpy.arange(0, 8)
X = 1 + exp(-1j * (pi / 4) * n) \
    - exp(-1j * (2 * pi / 4) * n) - exp(-1j * (3 * pi / 4) * n) \
    + exp(-1j * (4 * pi / 4) * n) + exp(-1j * (5 * pi / 4) * n) \
    - exp(-1j * (6 * pi / 4) * n) - exp(-1j * (7 * pi / 4) * n)
pprint(X)

# 6.25
print("6.25")
n = numpy.arange(0, 5)
X = (1 + (1 + 2j) * exp(1j * (2 * pi / 5) * n) 
     + (1 - 1j) * exp(1j * (4 * pi / 5) * n) + (1 + 1j) * exp(1j * (6 * pi / 5) * n) 
     + (1 - 2j) * exp(1j * (8 * pi / 5) * n)) / 5
pprint(X)

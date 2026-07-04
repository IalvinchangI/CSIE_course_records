import matplotlib.pyplot as plt
import numpy
from scipy.signal import convolve

SAMPLE = 25
RANGE = (0, SAMPLE)
CUTOFF = 50
n = numpy.linspace(RANGE[0], RANGE[1], SAMPLE)

def u(n: numpy.ndarray) -> numpy.ndarray:
    return n >= 0

# compute
x_n: numpy.ndarray = numpy.cos(n * numpy.pi / 6) * u(n)
h_n: numpy.ndarray = (0.6 ** n) * u(n)
y_n = convolve(x_n, h_n)


# plot
plt.stem(numpy.arange(len(y_n)), y_n)
plt.grid(True)
plt.xlim(-1, CUTOFF + 0.5)
plt.title(r"$y[n] = \sum_{k=-\infty}^{\infty} \cos(\frac{\pi k}{6}) u[k] (0.6)^{n-k} u[n-k]$", x=0.05, ha='left')
plt.xlabel("n")
plt.ylabel("y[n]")

# Save the plot as a jpg file and show it
plt.tight_layout()
plt.savefig("HW2_2_40.jpg")
plt.show()

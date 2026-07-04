from scipy import signal
from numpy import pi
from numpy import exp
import numpy
import matplotlib.pyplot as plt

SAMPLE = 512
RANGE = (0, SAMPLE)
# CUTOFF = 25

Zeros = [exp(0.48j * pi), exp(0.24j * pi), exp(-0.48j * pi), exp(-0.24j * pi)]
N_s, D_s = signal.zpk2tf(Zeros, [], 1)


# impulse response
x = numpy.zeros(SAMPLE)
x[0] = 1

h = signal.lfilter(N_s, D_s, x)

# FFT
H = numpy.fft.fft(h, 1024)
H_magnitude = numpy.abs(H)

# plot data
x = numpy.arange(RANGE[0], RANGE[1])* (500 / 1024)
y = H_magnitude[:SAMPLE]


# Plot (x, y) on 2 differnet figure (linear and logarithmeic scales)
# Add grid and the variable or proper name of x, y-axis
# Add a title
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 10))

# linear
ax1.plot(x, y)
ax1.set_xlabel('Frequency (Hz)')
ax1.set_ylabel('Magnitude')
ax1.grid(True)
ax1.set_title("Linear Scale", loc='left')

# logarithmeic
y_db = 20 * numpy.log10(y + 1e-12)
ax2.plot(x, y_db)
ax2.set_xlabel('Frequency (Hz)')
ax2.set_ylabel('Magnitude (dB)')
ax2.grid(True)
ax2.set_title("Logarithmic Scale (dB)", loc='left')


# output
# stack these two plot in one figure vertically (linear is at the top, logarithmeic is at the bottom)
plt.tight_layout()
plt.savefig("HW4_problem_X.jpg")
plt.show()
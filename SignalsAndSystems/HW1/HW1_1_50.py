import matplotlib.pyplot as plt
import numpy

SAMPLE = 1000
RANGE = (-2, 4)
t = numpy.linspace(RANGE[0], RANGE[1], SAMPLE)

# question (a)
plt.subplot(3, 1, 1)
x_t = 2 * numpy.maximum(0, t)

# Plot x_t without any decorations on data points in order to represent a continuous signal
# Create a title to represent the question number
plt.plot(t, x_t)
plt.grid(True)
plt.title(r"(a) $x(t) = 2r(t)$", x=0.05, ha='left')
plt.xlabel("t")
plt.ylabel("x(t)")


# question (b)
plt.subplot(3, 1, 2)
y_n = 5 * numpy.exp(-2 * t) * (t >= 0)

# Plot y_n without any decorations on data points in order to represent a continuous signal
# Create a title to represent the question number
plt.plot(t, y_n)
plt.grid(True)
# Using u(t) logic for the (t >= 0) step function equivalent in the formula
plt.title(r"(b) $y(t) = 5e^{-2t}u(t)$", x=0.05, ha='left')
plt.xlabel("t")
plt.ylabel("y(t)")


# question (c)
plt.subplot(3, 1, 3)
z_n = 4 * numpy.cos(4 * t) + 2 * numpy.sin(2 * t - numpy.pi / 4)

# Plot z_n without any decorations on data points in order to represent a continuous signal
# Create a title to represent the question number
plt.plot(t, z_n)
plt.grid(True)
plt.title(r"(c) $z(t) = 4\cos(4t) + 2\sin(2t - \frac{\pi}{4})$", x=0.05, ha='left')
plt.xlabel("t")
plt.ylabel("z(t)")


# output
plt.suptitle("1.50", x=0.05, ha='left')

# Save the plot as a jpg file and show it
plt.tight_layout()
plt.savefig("HW1_1_50.jpg")
plt.show()

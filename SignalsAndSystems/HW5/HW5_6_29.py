import numpy
import matplotlib.pyplot as plt

SAMPLE = 100
RANGE = (0, 2 * numpy.pi)

a = 0.75
o = numpy.linspace(RANGE[0], RANGE[1], SAMPLE)

x = (1 - a**2) / (1 - 2 * a * numpy.cos(o) + a**2)

y = numpy.abs(x)


# Plot y without any decorations on data points in order to represent a continuous signal
plt.plot(o, y)

# Add grid and the variable of x, y-axis
plt.grid(True, linestyle='--', alpha=0.7)
plt.xlabel(r'$\Omega$ (rad)')
plt.ylabel(r'$|X(\Omega)|$')

# Add a title showing question and the formula at the top-left corner of the plot
plt.title(r'$X(\Omega) = \frac{1 - a^2}{1 - 2a\cos(\Omega) + a^2}$  , where a = 0.75', loc='left')


# output
plt.savefig("HW5_6_29.jpg")
plt.show()

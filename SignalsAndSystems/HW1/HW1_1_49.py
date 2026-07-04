import matplotlib.pyplot as plt
import numpy

SAMPLE = 1000
RANGE = (0, SAMPLE)
CUTOFF = 25
n = numpy.linspace(RANGE[0], RANGE[1], SAMPLE)

# question (a)
plt.subplot(1, 2, 1)
x_n = 10 * (0.7 ** n)

# Create a stem plot (scatter plot with lines connected to the x-axis) to represent a discrete-time signal
# Only show the region where x is between -1 and CUTOFF + 0.5, and y is between -1 and x_n[0] + 1
# Create a title to represent the question number
plt.stem(n, x_n)
plt.grid(True)
plt.xlim(-1, CUTOFF + 0.5)
plt.ylim(-1, x_n[0] + 1)
plt.title(r"(a) $x[n] = 10(0.7)^n$", x=0.05, ha='left')
plt.xlabel("n")
plt.ylabel("x[n]")


# question (b)
plt.subplot(1, 2, 2)
y_n = 10 * (1.2 ** n)

# Create a stem plot (scatter plot with lines connected to the x-axis) to represent a discrete-time signal
# Only show the region where x is between -1 and CUTOFF + 0.5, and y is between -5 and y_n[CUTOFF] * (21 / 20)
# Create a title to represent the question number
plt.stem(n, y_n)
plt.grid(True)
plt.xlim(-1, CUTOFF + 0.5)
plt.ylim(-5, y_n[CUTOFF] * (21 / 20))
plt.title(r"(b) $y[n] = 10(1.2)^n$", x=0.05, ha='left')
plt.xlabel("n")
plt.ylabel("y[n]")


# output
plt.suptitle("1.49", x=0.05, ha='left')

# Save the plot as a jpg file and show it
plt.tight_layout()
plt.savefig("HW1_1_49.jpg")
plt.show()

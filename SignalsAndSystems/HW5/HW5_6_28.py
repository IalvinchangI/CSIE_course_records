from scipy.fft import fft
import numpy
import matplotlib.pyplot as plt

# SAMPLE = 25
# RANGE = (0, SAMPLE)
# CUTOFF = 25


# (a)
x_a = numpy.array([1 for _ in range(0, 12 + 1)])
y_a = numpy.abs(fft(x_a))

# (b)
x_b = numpy.array([i for i in range(0, 10 + 1)])
y_b = numpy.abs(fft(x_b))

# (c)
x_c = numpy.array([1] + [1 / i for i in range(1, 10 + 1)])
y_c = numpy.abs(fft(x_c))

# (d)
x_d = numpy.array([i * (0.8 ** i) for i in range(0, 10 + 1)])
y_d = numpy.abs(fft(x_d))


fig, axes = plt.subplots(2, 2, figsize=(10, 8))
plots_info = [
    (axes[0, 0], y_a, '(a)'),
    (axes[0, 1], y_b, '(b)'),
    (axes[1, 0], y_c, '(c)'),
    (axes[1, 1], y_d, '(d)')
]

# Plot y_? by a stem plot (scatter plot with lines connected to the x-axis) to represent a discrete-time signal
# Add grid and the variable of x, y-axis
# Add a title showing question at the top-left corner of the plot
for ax, step_y, title in plots_info:
    step_t = numpy.arange(len(step_y))
    ax.stem(step_t, step_y)
    ax.grid(True, linestyle='--', alpha=0.7)
    ax.set_xlabel('k')
    ax.set_ylabel('|X(k)|')
    ax.set_title(title, loc='left', fontweight='bold')


# output
# Display 4 plots at 4 corner in a single figure
plt.tight_layout()

# Save the plot as a jpg file and show it
plt.savefig("HW5_6_28.jpg")
plt.show()

from scipy import signal
import numpy
import matplotlib.pyplot as plt

# SAMPLE = 25
# RANGE = (0, SAMPLE)
CUTOFF = 25

N_s = [1, -2, 6, 1]
D_s = [1, -2, 0, 5, -1, 4]

system = signal.dlti(N_s, D_s)

unit_inside = (sum(D_s) != 0)
print(f"Is |z| = 1 inside the ROC? {unit_inside}")

print()
print(f"poles = {system.poles}")
poles_inside = numpy.all(numpy.abs(system.poles) < 1)
print(f"Are all poles inside the unit circle? {poles_inside}")

print()
print(f"Stability: {unit_inside and poles_inside}")


# step response
step_t, step_y = signal.dimpulse(system, n=CUTOFF)
step_y = step_y[0][:, 0]

# Plot poles on Complex plane
plt.scatter(
    numpy.real(system.poles),
    numpy.imag(system.poles),
    marker='x',
    s=100,
    label='Poles'
)
for pole in system.poles:
    plt.annotate(
        f"({pole.real:.3f}, {pole.imag:.3f})",
        (pole.real, pole.imag),
        textcoords="offset points",
        xytext=(10, 10)
    )

# Plot unit circle
theta = numpy.linspace(0, 2 * numpy.pi, 400)
plt.plot(numpy.cos(theta), numpy.sin(theta), c="red", label='Unit Circle')

# Add grid
plt.grid(True)
plt.axhline(0, c="black")
plt.axvline(0, c="black")
plt.axis('equal')
plt.legend()

# Add a title
plt.title("Poles of H(z)", loc='left')


# output
plt.savefig("HW4_7_50.jpg")
plt.show()

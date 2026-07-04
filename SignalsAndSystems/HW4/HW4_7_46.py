from scipy import signal
import matplotlib.pyplot as plt

# SAMPLE = 25
# RANGE = (0, SAMPLE)
CUTOFF = 25

N_s = [1, 6, 1]
D_s = [1, 3, 0, 4, 10]

system = signal.dlti(N_s, D_s)


# step response
step_t, step_y = signal.dstep(system, n=CUTOFF)
step_y = step_y[0][:, 0]

# Plot (step_t, step_y) by a stem plot (scatter plot with lines connected to the x-axis) to represent a discrete-time signal
# Add grid and the variable of x, y-axis
# Add a title showing question at the top-left corner of the plot
plt.stem(step_t, step_y)
plt.xlabel('n')
plt.ylabel('Step Response')
plt.grid(True)
plt.title("Step Response of H(z)", loc='left')


# output
plt.savefig("HW4_7_46.jpg")
plt.show()

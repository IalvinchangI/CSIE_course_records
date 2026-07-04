from scipy import signal
import matplotlib.pyplot as plt

# SAMPLE = 25
# RANGE = (0, SAMPLE)
# CUTOFF = 50

N_s = [2, 5]
D_s = [1, 5, 6]  # (s + 2)(s + 3)

system = signal.TransferFunction(N_s, D_s)


# (a) impulse response
impulse_t, impulse_y = signal.impulse(system)

# Plot (impulse_t, impulse_y) without any decorations on data points in order to represent a continuous signal
# Add grid and the variable of x, y-axis
# Add a title showing the question number and its text in the top-left corner of the plot
plt.figure(figsize=(12, 6))
plt.subplot(1, 2, 1)
plt.plot(impulse_t, impulse_y)
plt.xlabel('t')
plt.ylabel('Impulse Response')
plt.grid(True)
plt.title("(a) Impulse Response", loc='left')


# (b) step response
step_t, step_y = signal.step(system)

# Plot (step_t, step_y) without any decorations on data points in order to represent a continuous signal
# Add grid and the variable of x, y-axis
# Add a title showing the question number and its text in the top-left corner of the plot
plt.subplot(1, 2, 2)
plt.plot(step_t, step_y)
plt.xlabel('t')
plt.ylabel('Step Response')
plt.grid(True)
plt.title("(b) Step Response", loc='left')


# output
# Display two plots side by side within a single figure, and place ‘3.49’ as a super title at the top-left corner
# Save the plot as a jpg file and show it
plt.suptitle("3.49", x=0.01, ha='left')
plt.tight_layout()
plt.savefig("HW3_3_49.jpg")
plt.show()

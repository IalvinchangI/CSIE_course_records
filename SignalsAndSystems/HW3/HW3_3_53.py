from scipy import signal
import matplotlib.pyplot as plt

# (a)
N_s_a = [1, 10, 0]     # s * (s + 10)
D_s_a = [1, 70, 1000]  # (s + 20) * (s + 50)

system_a = signal.TransferFunction(N_s_a, D_s_a)
w_a, mag_a, phase_a = signal.bode(system_a)

# Plot (w_a, mag_a) without any decorations on data points in order to represent a continuous signal
# Add grid and the variable of x, y-axis
# Add a title showing the question number and its text in the top-left corner of the plot
# Use LaTeX to display the mathematical function of H(s)
plt.figure(figsize=(14, 10))
plt.subplot(2, 2, 1)
plt.semilogx(w_a, mag_a)
plt.grid(True, which="both")
plt.xlabel('Frequency (rad/s)')
plt.ylabel('Magnitude (dB)')
plt.title(r"(a) $H(s) = \frac{s(s+10)}{(s+20)(s+50)}$", loc='left')

# Plot (w_a, phase_a) without any decorations on data points in order to represent a continuous signal
# Add grid and the variable of x, y-axis
# Display these two plots vertically within a single figure
plt.subplot(2, 2, 3)
plt.semilogx(w_a, phase_a)
plt.grid(True, which="both")
plt.xlabel('Frequency (rad/s)')
plt.ylabel('Phase (deg)')


# (b)
N_s_b = [1, 1]
D_s_b = [1, 24.5, 61, 32]  # (s + 2) * (s^2 + 22.5s + 16) = s^3 + 24.5s^2 + 61s + 32

system_b = signal.TransferFunction(N_s_b, D_s_b)
w_b, mag_b, phase_b = signal.bode(system_b)

# Plot (w_b, mag_b) without any decorations on data points in order to represent a continuous signal
# Add grid and the variable of x, y-axis
# Add a title showing the question number and its text in the top-left corner of the plot
# Use LaTeX to display the mathematical function of H(s)
plt.subplot(2, 2, 2)
plt.semilogx(w_b, mag_b)
plt.grid(True, which="both")
plt.xlabel('Frequency (rad/s)')
plt.ylabel('Magnitude (dB)')
plt.title(r"(b) $H(s) = \frac{s+1}{(s+2)(s^2+22.5s+16)}$", loc='left')

# Plot (w_b, phase_b) without any decorations on data points in order to represent a continuous signal
# Add grid and the variable of x, y-axis
# Display these two plots vertically within a single figure
plt.subplot(2, 2, 4)
plt.semilogx(w_b, phase_b)
plt.grid(True, which="both")
plt.xlabel('Frequency (rad/s)')
plt.ylabel('Phase (deg)')


# output
# Display two figures side by side within a single figure, and place ‘3.53’ as a super title at the top-left corner
# Save the plot as a jpg file and show it
plt.suptitle("3.53", x=0.01, ha='left', fontsize=16)
plt.tight_layout(rect=[0, 0.03, 1, 0.95]) # Adjust layout to make room for suptitle
plt.savefig("HW3_3_53.jpg")
plt.show()

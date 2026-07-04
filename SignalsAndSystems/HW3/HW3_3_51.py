from scipy import signal

print("3.51")

# (a)
N_s = [1, -2]
D_s = [1, 2, 10]  # (s + 1)^2 + 9 = s^2 + 2s + 10

system = signal.TransferFunction(N_s, D_s)

print("(a)")
print(f"\tzeros = {system.zeros}")
print(f"\tpoles = {system.poles}")


# (b)
N_s = [1, 2, 5]
D_s = [1, 4, 13, 0]

system = signal.TransferFunction(N_s, D_s)

print("(b)")
print(f"\tzeros = {system.zeros}")
print(f"\tpoles = {system.poles}")


# (c)
N_s = [1, 10, 5]
D_s = [1, 4, 10, 6]

system = signal.TransferFunction(N_s, D_s)

print("(c)")
print(f"\tzeros = {system.zeros}")
print(f"\tpoles = {system.poles}")

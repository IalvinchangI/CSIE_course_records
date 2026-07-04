from scipy import signal

N_s = [1, 6, 1]
D_s = [1, 3, 0, 4, 10]

system = signal.dlti(N_s, D_s)


print(f"\tzeros = {system.zeros}")
print(f"\tpoles = {system.poles}")

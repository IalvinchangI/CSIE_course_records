from scipy.signal import residue
import sympy
from sympy.abc import t

def inverseLaplaceSymbol(residues, poles, t):
    output = 0

    # fraction
    count = 0
    previous_pole = None
    for i in range(len(residues)):
        temp = residues[i] * sympy.exp(poles[i] * t)

        if previous_pole == poles[i]:
            count += 1
            temp *= t ** count
        else:
            count = 0

        previous_pole = poles[i]
        output += temp

    return output

N_s = [1, 6, 10]
D_s = [1, 7, 11, 5]

residues, poles, direct_terms = residue(N_s, D_s)

inverse_Laplace = inverseLaplaceSymbol(residues, poles, t)

print("3.46")
print(f"{residues = }")
print(f"{poles = }")
print(f"{direct_terms = }")
print(f"{inverse_Laplace = }")

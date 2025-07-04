from qiskit import QuantumRegister, ClassicalRegister, QuantumCircuit
from QuantumTool import bell_inequality_circuits, run_circuits, calculate_bell_inequality_result, calculate_negativity
from QuantumTool import state_tomography_experiment, run_experiment, extract_state_tomography_result
from FormalOutput import circuit_diagram, plot_density_matrix, str_density_matrix, output_pauli_counts
from Load import load_backend, print_errors_data, TOKEN

import os
os.chdir(r"IntroductionToQuantumComputation\BellInequality")



# save result
experiment_data_filename = "BellInequality_experiment_data.txt"
error_data_filename = "BellInequality_error_data.txt"
pauli_counts_filename = "BellInequality_pauli_counts.csv"
bell_inequality_circuit_diagram_figname = "BellInequality_circuit_diagram.jpg"
entanglement_circuit_diagram_figname = "Entanglement_circuit_diagram.jpg"
density_matrix_figname = "BellInequality_density_matrix.jpg"

# setup
simulation_TF = True
physical_qubits = [123, 124]
device = "ibm_sherbrooke"
shots = 8192


# Load Backend
backend = load_backend(TOKEN, simulation_TF=simulation_TF, backend_name=device)
print("Load Backend Complete")


# Entanglement Circuit Design
def Entanglement():
    qreg_q = QuantumRegister(2, 'q')
    creg_c = ClassicalRegister(2, 'c')
    circuit = QuantumCircuit(qreg_q, creg_c)

    circuit.reset(qreg_q[0])
    circuit.reset(qreg_q[1])
    circuit.h(qreg_q[0])
    circuit.cx(qreg_q[0], qreg_q[1])
    # @columns [0,0,1,2]
    return circuit

# State Tomography Entanglement
StateTomographyEntanglement = run_experiment(backend, shots=shots)(
    circuit_diagram(savefig_name=entanglement_circuit_diagram_figname)(
        state_tomography_experiment(backend, physical_qubits=physical_qubits, measurement_indices=[0, 1])(
            Entanglement
        )
    )
)

StateTomography_data = StateTomographyEntanglement()
print("State Tomography Complete")

# Bell Inequality
BellInequality = run_circuits(backend, physical_qubits=physical_qubits, shots=shots)(
    circuit_diagram(savefig_name=bell_inequality_circuit_diagram_figname)(
        bell_inequality_circuits(
            Entanglement
        )
    )
)

BellInequality_data = BellInequality()
print("Bell Inequality Complete")


# Extract Result
bell_inequality = calculate_bell_inequality_result(BellInequality_data)
density_matrix, pauli_counts = extract_state_tomography_result(StateTomography_data)
negativity = calculate_negativity(density_matrix)
if experiment_data_filename == None:                          # print
    print(f"{device = }")
    print(f"{shots = }")
    print(f"{bell_inequality = }")
    print(f"density_matrix =\n{str_density_matrix(density_matrix)}")
    print(f"{pauli_counts = }")
    print(f"{negativity = }")
else:                                                         # save
    with open(experiment_data_filename, mode="w") as file:
        print(f"{physical_qubits = }", file=file)
        print(f"{device = }", file=file)
        print(f"{shots = }", file=file)
        print(f"{bell_inequality = }", file=file)
        print(f"density_matrix =\n{str_density_matrix(density_matrix)}", file=file)
        print(f"{pauli_counts = }", file=file)
        print(f"{negativity = }", file=file)

plot_density_matrix(density_matrix, savefig_name=density_matrix_figname)
output_pauli_counts(pauli_counts, savefile_name=pauli_counts_filename)

print_errors_data(
    backend, 
    physical_qubits, [list(pair) for pair in zip(physical_qubits[1:], physical_qubits)], 
    filename=error_data_filename
)


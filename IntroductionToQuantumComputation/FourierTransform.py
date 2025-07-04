from qiskit import QuantumRegister, ClassicalRegister, QuantumCircuit
from QuantumTool import state_tomography_experiment, run_experiment, extract_state_tomography_result
from FormalOutput import circuit_diagram, plot_density_matrix, str_density_matrix, output_pauli_counts
from Load import load_backend, print_errors_data, TOKEN
from numpy import pi

import os
os.chdir(r"IntroductionToQuantumComputation\FourierTransform")



# save result
experiment_data_filename = "FourierTransform_experiment_data.txt"
error_data_filename = "FourierTransform_error_data.txt"
pauli_counts_filename = "FourierTransform_pauli_counts.csv"
circuit_diagram_figname = "FourierTransform_circuit_diagram.jpg"
density_matrix_figname = "FourierTransform_density_matrix.jpg"

# setup
simulation_TF = True
physical_qubits = [122, 123, 124]
device = "ibm_sherbrooke"
shots = 8192


# Load Backend
backend = load_backend(TOKEN, simulation_TF=simulation_TF, backend_name=device)
print("Load Backend Complete")


# State Tomography
@run_experiment(backend, shots=shots)
@circuit_diagram(savefig_name=circuit_diagram_figname)
@state_tomography_experiment(backend, physical_qubits=physical_qubits, measurement_indices=[0, 1, 2])
@circuit_diagram(savefig_name="original_" + circuit_diagram_figname)
def FourierTransform():
    qreg_q = QuantumRegister(3, 'q')
    creg_c = ClassicalRegister(3, 'c')
    circuit = QuantumCircuit(qreg_q, creg_c)

    circuit.reset(qreg_q[0])
    circuit.reset(qreg_q[1])
    circuit.reset(qreg_q[2])
    circuit.h(qreg_q[0])
    circuit.cx(qreg_q[0], qreg_q[1])
    circuit.cx(qreg_q[1], qreg_q[2])
    circuit.h(qreg_q[0])
    circuit.cp(pi / 2, qreg_q[1], qreg_q[0])
    circuit.cp(pi / 4, qreg_q[2], qreg_q[0])
    circuit.h(qreg_q[1])
    circuit.cp(pi / 2, qreg_q[2], qreg_q[1])
    circuit.h(qreg_q[2])
    # @columns [0,0,0,1,2,3,4,5,6,7,8,9]
    return circuit

data = FourierTransform()
print("State Tomography Complete")


# Extract Result
density_matrix, pauli_counts = extract_state_tomography_result(data)
if experiment_data_filename == None:                          # print
    print(f"{physical_qubits = }")
    print(f"{device = }")
    print(f"{shots = }")
    print(f"density_matrix =\n{str_density_matrix(density_matrix)}")
    print(f"{pauli_counts = }")
else:                                                         # save
    with open(experiment_data_filename, mode="w") as file:
        print(f"{physical_qubits = }", file=file)
        print(f"{device = }", file=file)
        print(f"{shots = }", file=file)
        print(f"density_matrix =\n{str_density_matrix(density_matrix)}", file=file)
        print(f"{pauli_counts = }", file=file)

plot_density_matrix(density_matrix, savefig_name=density_matrix_figname)
output_pauli_counts(pauli_counts, savefile_name=pauli_counts_filename)

print_errors_data(
    backend, 
    physical_qubits, [list(pair) for pair in zip(physical_qubits[1:], physical_qubits)], 
    filename=error_data_filename
)

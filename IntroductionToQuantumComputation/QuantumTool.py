from qiskit_experiments.library import StateTomography
from qiskit import generate_preset_pass_manager, transpile
from qiskit_ibm_runtime import SamplerV2 as Sampler
from qiskit.quantum_info import Statevector, DensityMatrix, state_fidelity, negativity
import numpy

import functools
from collections import defaultdict

from qiskit import QuantumCircuit
from qiskit.result import Result
from qiskit.primitives.containers import PrimitiveResult
from qiskit_experiments.framework import ExperimentData


__BELL_INEQUALITY_CIRCUITS_NAMES = ("QS", "RS", "RT", "QT")



def bell_inequality_circuits(algorithm):
    """
    generate bell's inequality circuits (decorator)
    
    Returns
    -------
        a dictionary of circuits, where the key is the observable of the circuit
    """
    @functools.wraps(algorithm)
    def temp(*args, **kargs) -> Result:
        # Circuit for <QS>: Alice Q=Z, Bob S=measure at pi/4
        qc_qs: QuantumCircuit = algorithm(*args, **kargs)
        # Q: Z on q0 (no change)
        qc_qs.ry(-numpy.pi/4, 1) # Bob measures S
        qc_qs.measure([0,1], [0,1]) # q0 to c0, q1 to c1. Output "c1c0"
        qc_qs.name = __BELL_INEQUALITY_CIRCUITS_NAMES[0]
        qc_qs.metadata = {"name": qc_qs.name}

        # Circuit for <RS>: Alice R=X, Bob S=measure at pi/4
        qc_rs: QuantumCircuit = algorithm(*args, **kargs)
        qc_rs.h(0) # Alice measures R=X
        qc_rs.ry(-numpy.pi/4, 1) # Bob measures S
        qc_rs.measure([0,1], [0,1])
        qc_rs.name = __BELL_INEQUALITY_CIRCUITS_NAMES[1]
        qc_rs.metadata = {"name": qc_rs.name}

        # Circuit for <RT>: Alice R=X, Bob T=measure at 3pi/4
        qc_rt: QuantumCircuit = algorithm(*args, **kargs)
        qc_rt.h(0) # Alice measures R=X
        qc_rt.ry(-3*numpy.pi/4, 1) # Bob measures T
        qc_rt.measure([0,1], [0,1])
        qc_rt.name = __BELL_INEQUALITY_CIRCUITS_NAMES[2]
        qc_rt.metadata = {"name": qc_rt.name}

        # Circuit for <QT>: Alice Q=Z, Bob T=measure at 3pi/4
        qc_qt: QuantumCircuit = algorithm(*args, **kargs)
        # Q: Z on q0 (no change)
        qc_qt.ry(-3*numpy.pi/4, 1) # Bob measures T
        qc_qt.measure([0,1], [0,1])
        qc_qt.name = __BELL_INEQUALITY_CIRCUITS_NAMES[3]
        qc_qt.metadata = {"name": qc_qt.name}

        return {
            qc_qs.name : qc_qs, 
            qc_rs.name : qc_rs, 
            qc_rt.name : qc_rt, 
            qc_qt.name : qc_qt
        }
    return temp


def state_tomography_experiment(backend, physical_qubits=None, measurement_indices=None):
    """
    generate state tomography experiment (decorator)

    Parameters
    ----------
    backend : Backend
        the backend to run on.
    physical_qubits : List[Int]
        Specifies the physical qubits.
        If None, this will be qubits [0, N) for an N-qubit circuit.
        The default value is None.
    measurement_indices : List[Int]
        Specifies the physical_qubits indices to be measured.
        If None, all circuit physical qubits will be measured.
        The default value is None.
    
    Returns
    -------
        state tomography experiment
    """
    def decorator(algorithm):
        @functools.wraps(algorithm)
        def temp(*args, **kargs) -> Result:
            circuit: QuantumCircuit = algorithm(*args, **kargs)

            # setup tomography experiment
            experiment = StateTomography(circuit, backend=backend, physical_qubits=physical_qubits, measurement_indices=measurement_indices)

            return experiment
        return temp
    return decorator


def run_experiment(backend, shots=2048):
    """
    run experiment (decorator)

    Parameters
    ----------
    backend : Backend
        the backend to run on.
    shots : Int
        Specifies the number of shots.
        The default value is 2048.
    
    Returns
    -------
        ExperimentData
    """
    def decorator(algorithm_experiment):
        @functools.wraps(algorithm_experiment)
        def temp(*args, **kargs) -> Result:
            experiment = algorithm_experiment(*args, **kargs)

            # Execute the circuit on backend
            data = experiment.run(backend=backend, shots=shots).block_for_results()

            return data
        return temp
    return decorator


def run_circuits(backend, physical_qubits=None, shots=2048):
    """
    execute circuits (decorator)

    Parameters
    ----------
    backend : Backend
        the backend to run on.
    shots : Int
        Specifies the number of shots.
        The default value is 2048.
    
    Returns
    -------
        Result
    """
    def decorator(algorithm):
        @functools.wraps(algorithm)
        def temp(*args, **kargs) -> PrimitiveResult:
            result = algorithm(*args, **kargs)

            if isinstance(result, dict):
                circuits = list(result.values())
            else:
                circuits = result

            # compile the circuit down to low-level QASM instructions
            # supported by the backend (not needed for simple circuits)
            # compiled_circuit = transpile(circuits, backend=backend, initial_layout=physical_qubits)
            pm = generate_preset_pass_manager(backend=backend, optimization_level=1, initial_layout=physical_qubits)
            compiled_circuit = pm.run(circuits)

            # Execute the circuit on the qasm simulator
            sampler = Sampler(backend)
            job = sampler.run(compiled_circuit, shots=shots)

            # Grab results from the job
            return job.result()
        return temp
    return decorator


def extract_state_tomography_result(experiment_data: ExperimentData):
    """
    extract the density matrix and the measurement counts in different Pauli bases

    Parameters
    ----------
    experiment_data : ExperimentData
        the experimental result of the state tomography
    
    Returns
    -------
        density_matrix: DensityMatrix, pauli_counts: dict
    """

    M_IDX_2_BASIS = {0: "Z", 1: "X", 2: "Y"}
    density_matrix: DensityMatrix = experiment_data.analysis_results(index="state", dataframe=True).iloc[0].value

    pauli_counts = defaultdict(list)
    for data_dict in experiment_data.data():
        metadata = data_dict["metadata"]

        # counts
        clbits = metadata["clbits"]
        counts = dict(map(
            lambda x: (
                "".join(x[0][len(x[0]) - 1 - i] for i in reversed(clbits)), 
                x[1]
            ), 
            data_dict["counts"].items()
        ))
        
        # basis
        basis = "".join(M_IDX_2_BASIS[i] for i in metadata["m_idx"])[::-1]

        # store
        pauli_counts[basis].append(counts)
        # print(metadata, data_dict["counts"])
    
    return density_matrix, dict(pauli_counts)


def calculate_fidelity(ideal_state_label: str, real_density_matrix: DensityMatrix):
    """
    calculate fidelity between ideal_density_matrix and real_density_matrix

    Parameters
    ----------
    ideal_state_label : str
        a eigenstate string ket label
    real_density_matrix : DensityMatrix
        a density matrix from experiment
    
    Returns
    -------
        Fidelity(ideal_density_matrix, real_density_matrix)
    """
    
    ideal_state_vector = Statevector.from_label(ideal_state_label)
    ideal_density_matrix = DensityMatrix(ideal_state_vector)

    return state_fidelity(ideal_density_matrix, real_density_matrix)


def __calculate_expectation_value(counts, shots):
    p00 = counts.get('00', 0) / shots
    p01 = counts.get('01', 0) / shots
    p10 = counts.get('10', 0) / shots
    p11 = counts.get('11', 0) / shots
    return p00 - p10 - p01 + p11

def calculate_bell_inequality_result(result: PrimitiveResult):
    """
    calculate the expectaion value of bell inequality

    Parameters
    ----------
    result : PrimitiveResult
        the job result after running run_circuit
    
    Returns
    -------
        the result expectaion of bell inequality
    """
    name2counts = dict()
    try:
        for r in result:
            if r.metadata["name"] not in __BELL_INEQUALITY_CIRCUITS_NAMES:
                raise AttributeError()
            name2counts[r.metadata["name"]] = (r.data.c.get_counts(), r.data.c.shots)
    except (AttributeError, KeyError) as e:
        print("Switch to the static constant name sequence when calculating the Bell inequality result.")
        name2counts = dict(zip(
            __BELL_INEQUALITY_CIRCUITS_NAMES, 
            [(r.data.c.get_counts() , r.data.c.num_shots) for r in result]
        ))
    
    expectations = {
        name : __calculate_expectation_value(counts, shots) 
        for name, (counts, shots) in name2counts.items()
    }
    return abs(expectations["QS"] + expectations["RS"] + expectations["RT"] - expectations["QT"])


def calculate_negativity(density_matrix: DensityMatrix):
    """
    calculate negativity of density_matrix using the trace of B

    Parameters
    ----------
    density_matrix : DensityMatrix
        a density matrix
    
    Returns
    -------
        Negativity(density_matrix)
    """
    return negativity(density_matrix, [0])

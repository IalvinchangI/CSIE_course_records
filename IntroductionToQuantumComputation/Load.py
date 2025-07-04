from qiskit_ibm_runtime import QiskitRuntimeService
from qiskit_aer import AerSimulator
from pandas import DataFrame

from qiskit_ibm_runtime import IBMBackend



from IBMQ_API_Token import TOKEN as __TOKEN
TOKEN = __TOKEN
""" IBMQ API token """

def load_backend(token, simulation_TF=True, backend_name="ibm_brisbane"):
    """
    load backend, that is, load the device that run the circuit

    Parameters
    ----------
    token : str
        your IBMQ API token
    simulation_TF : bool
        Whether to use simulator.
        The default value is True.
    backend_name : str
        Specifies the backend by name.
        If None, this will return AerSimulator object without any configuration.
        The default value is "ibm_brisbane".
    """

    # local simulation without any configuration
    if backend_name == None:
        return AerSimulator()

    # using IBMQ
    service = QiskitRuntimeService(channel="ibm_quantum", token=token)
    backend = service.backend(backend_name)

    if simulation_TF == True:
        return AerSimulator.from_backend(backend)
    else:
        return backend


def print_errors_data(backend: IBMBackend, physical_qubits=None, physical_connections=None, filename=None):
    """
    print all neccessary

    Parameters
    ----------
    token : str
        your IBMQ API token
    simulation_TF : bool
        Whether to use simulator.
        The default value is True.
    backend_name : str
        Specifies the backend by name.
        If None, this will return AerSimulator object without any configuration.
        The default value is "ibm_brisbane".
    filename : str | None
        Specifies the filename for saving the error data.
        If None, the error data will be shown on screen instead.
        The default value is None.
    """
    
    properties = backend.properties()
    if physical_qubits == None:
        physical_qubits = list(range(backend.configuration().n_qubits))
    if physical_connections == None:
        physical_connections = backend.configuration().coupling_map


    # Date
    try:
        last_update_date = properties.last_update_date.isoformat()
    except Exception:
        last_update_date = None

    # Single-Qubit Gate Errors
    sx_gate_error = dict()
    for qubit in physical_qubits:
        try:
            error = properties.gate_error('sx', qubit)
            sx_gate_error[qubit] = error
        except Exception:
            pass
    x_gate_error = dict()
    for qubit in physical_qubits:
        try:
            error = properties.gate_error('x', qubit)
            x_gate_error[qubit] = error
        except Exception:
            pass
    id_gate_error = dict()
    for qubit in physical_qubits:
        try:
            error = properties.gate_error('id', qubit)
            id_gate_error[qubit] = error
        except Exception:
            pass
    single_gate_error = DataFrame(
        [sx_gate_error, x_gate_error, id_gate_error]
    ).transpose()
    single_gate_error.columns=["SX Gate Error", "X Gate Error", "ID Gate Error"]

    # CNOT Gate Errors (ECR Gate Errors)
    ecr_gate_error = dict()
    for gate in filter(lambda x: x["qubits"] in physical_connections and x["gate"] == "ecr", properties.to_dict()["gates"]):
        pair = tuple(gate["qubits"])
        error = tuple(filter(lambda x: x["name"]  == "gate_error", gate["parameters"]))[0]["value"]
        ecr_gate_error[pair] = error
    cnot_gate_error = DataFrame(
        [ecr_gate_error]
    ).transpose()
    cnot_gate_error.columns=["ECR Gate Error"]

    # Readout Assignment Errors
    readout_assignment_error_m0p1 = dict()
    readout_assignment_error_m1p0 = dict()
    for qubit in physical_qubits:
        try:
            prop_dict = properties.to_dict()['qubits'][qubit]
            p_meas0_prep1 = next((p['value'] for p in prop_dict if p['name'] == 'prob_meas0_prep1'), None)
            p_meas1_prep0 = next((p['value'] for p in prop_dict if p['name'] == 'prob_meas1_prep0'), None)
            
            if p_meas0_prep1 is not None:
                readout_assignment_error_m0p1[qubit] = p_meas0_prep1
            if p_meas1_prep0 is not None:
                readout_assignment_error_m1p0[qubit] = p_meas1_prep0
        except Exception:
            pass
    readout_assignment_error = DataFrame(
        [readout_assignment_error_m0p1, readout_assignment_error_m1p0]
    ).transpose()
    readout_assignment_error.columns=["P(measure 0 | prepare 1)", "P(measure 1 | prepare 0)"]
    readout_assignment_error["Readout Assignment Errors"] = readout_assignment_error.mean(axis=1)
    

    # output
    if filename == None:
        print(f"last_update_date = {last_update_date}")
        for qubit, error in sx_gate_error.items():
            print(f"Qubit {qubit} 'sx' gate error: {error:11.9f}")
        for qubit, error in x_gate_error.items():
            print(f"Qubit {qubit} 'x' gate error: {error:11.9f}")
        for qubit, error in id_gate_error.items():
            print(f"Qubit {qubit} 'id' gate error: {error:11.9f}")
        for pair, error in ecr_gate_error.items():
            print(f"ECR error ({pair[0]}-{pair[1]}): {error:11.9f}")
        for qubit in readout_assignment_error.index:
            print(f"Qubit {qubit}: " + 
                  f"P(measure 0 | prepare 1) = {readout_assignment_error.loc[qubit]["P(measure 0 | prepare 1)"]:11.9f}, " + 
                  f"P(measure 1 | prepare 0) = {readout_assignment_error.loc[qubit]["P(measure 1 | prepare 0)"]:11.9f}, " + 
                  f"Readout Assignment Errors = {readout_assignment_error.loc[qubit]["Readout Assignment Errors"]:11.9f}", file=file
            )
    else:
        with open(filename, mode="w") as file:
            print(f"last_update_date = {last_update_date}", file=file)
            for qubit, error in sx_gate_error.items():
                print(f"Qubit {qubit} 'sx' gate error: {error:11.9f}", file=file)
            for qubit, error in x_gate_error.items():
                print(f"Qubit {qubit} 'x' gate error: {error:11.9f}", file=file)
            for qubit, error in id_gate_error.items():
                print(f"Qubit {qubit} 'id' gate error: {error:11.9f}", file=file)
            for pair, error in ecr_gate_error.items():
                print(f"ECR error ({pair[0]}-{pair[1]}): {error:11.9f}", file=file)
            for qubit in readout_assignment_error.index:
                print(f"Qubit {qubit}: " + 
                      f"P(measure 0 | prepare 1) = {readout_assignment_error.loc[qubit]["P(measure 0 | prepare 1)"]:11.9f}, " + 
                      f"P(measure 1 | prepare 0) = {readout_assignment_error.loc[qubit]["P(measure 1 | prepare 0)"]:11.9f}, " + 
                      f"Readout Assignment Errors = {readout_assignment_error.loc[qubit]["Readout Assignment Errors"]:11.9f}", file=file
                )
        
        single_gate_error.to_csv(filename.replace(".", "_single."))
        cnot_gate_error.to_csv(filename.replace(".", "_cnot."))
        readout_assignment_error.to_csv(filename.replace(".", "_readout."))
        

from qiskit_experiments.library import StateTomography
from qiskit.visualization import plot_state_city
import pandas
import matplotlib.pyplot as plt
import functools

from qiskit import QuantumCircuit
from qiskit.quantum_info import DensityMatrix
from matplotlib.figure import Figure



def plot_density_matrix(density_matrix: DensityMatrix, savefig_name=None):
    """
    show or save the density matrix

    Parameters
    ----------
    density_matrix : DensityMatrix
        the target density matrix
    savefig_name : str | None
        Specifies the filename for saving the circuit diagram.
        If None, the diagram will be shown on screen instead.
        The default value is None.
    """
    
    figure: Figure = plot_state_city(density_matrix)
    if savefig_name == None:  # show
        plt.show()
    else:  # save
        figure.savefig(savefig_name)


def str_density_matrix(density_matrix: DensityMatrix):
    """
    replace j to i in density matrix
    
    Parameters
    ----------
    density_matrix : DensityMatrix
        the target density matrix
    """
    return str(density_matrix).replace("j", "i")


def output_pauli_counts(pauli_counts: dict, savefile_name=None):
    """
    return or save the pauli counts in a pandas DataFrame

    Parameters
    ----------
    pauli_counts : dict
        the target pauli_counts
    savefig_name : str | None
        Specifies the filename for saving the pauli counts.
        If None, the pauli counts will be returned instead.
        The default value is None.
    
    Returns
    -------
        return pauli counts in a pandas DataFrame format
    """
    output = pandas.DataFrame()
    for name, (diction, ) in pauli_counts.items():
        new_cols = set(diction.keys()) - set(output.columns)
        for col in new_cols:
            output[col] = pandas.NA
            
        output.loc[name] = diction
    output = output.fillna(0)

    if savefile_name == None:
        return output
    else:
        output.to_csv(savefile_name)


def circuit_diagram(savefig_name=None, return_fig_TF=False):
    """
    show or save the resulting histogram (decorator)

    Parameters
    ----------
    savefig_name : str | None
        Specifies the filename for saving the circuit diagram.
        If None, the diagram will be shown on screen instead.
        The default value is None.
    return_fig_TF : bool
        Whether to return the figure of circuit diagram.
        The default value is False.
    
    Returns
    -------
        1. QuantumCircuit if the output of the previous function is circuit
        2. StateTomography if the output of the previous function is StateTomography
        3. Figure if return_fig_TF is True
    """
    def decorator(algorithm):
        @functools.wraps(algorithm)
        def temp(*args, **kargs):
            result = algorithm(*args, **kargs)
            if isinstance(result, dict):
                for name, circuit in result.items():
                    figure: Figure = circuit.draw('mpl')
                    if savefig_name != None:  # save
                        figure.savefig(savefig_name.replace(".", f"_{name}.", 1))
            else:
                if isinstance(result, StateTomography):
                    circuits = result.circuits()
                elif isinstance(result, QuantumCircuit):
                    circuits = [result]
                else:
                    circuits = result

                # Draw the circuit
                for i, circuit in enumerate(circuits, 1):
                    figure: Figure = circuit.draw('mpl')
                    if savefig_name != None:  # save
                        figure.savefig(savefig_name.replace(".", f"{i}.", 1))
            if savefig_name == None:  # show
                plt.show()
            
            if return_fig_TF == True:
                return figure
            else:
                return result
        return temp
    return decorator

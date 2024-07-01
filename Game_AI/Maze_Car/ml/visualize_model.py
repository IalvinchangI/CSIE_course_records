import matplotlib.pyplot as plt
import numpy
from collect import ML_Data
import os



MODEL_NAME = "tree_40_5_NoPos"
MODEL_PATH = r".\model"

DATA_PATH = r".\data"



# load data
def data_getter(data: list) -> numpy.ndarray:
    # condition
    condition: numpy.ndarray = numpy.hstack((
        # data[0]['endingPos'], 
        # data[0]['selfPos'], 
        (data[0]['selfAngle'], ), 
        data[0]['sensor']
    ))
    # condition: numpy.ndarray = numpy.ndarray
    # action
    action: numpy.ndarray = numpy.array((
        data[1]['left_PWM'], 
        data[1]['right_PWM']
    )).flatten()

    return condition, action

conditions: list = list()
actions: list = list()
for file_name in os.listdir(DATA_PATH):
    with ML_Data(DATA_PATH, file_name) as file:
        for data in file.read():
            condition, action = data_getter(data)
            conditions.append(condition)
            actions.append(action)
conditions = numpy.array(conditions)
actions = numpy.array(actions)

# load model
with ML_Data(MODEL_PATH, MODEL_NAME + ".pickle") as file:
    model = file.read()


# plot
VERTICAL_NUMBER: int = 2#len(actions[0])
# HORIZONTAL_NUMBER: int = len(conditions[0])

START = 1000
END = 3000
conditions = conditions[START:END]
actions = actions[START:END]


predictions = model.predict(conditions)

correct = 0
for pre, act in zip(predictions, actions):
    if (pre == act).all():
        correct += 1
print(correct / len(predictions))


def plot_subplot(number: int, index: int, x_name: str = "x") -> None:
    plt.figure(number)
    plt.subplot(VERTICAL_NUMBER, 1, 1)
    plt.scatter(conditions[:, index], actions[:, 0], c="gold")
    plot_x, plot_y = zip(*sorted(zip(conditions[:, index], predictions[:, 0]), key=lambda x: x[0]))
    plt.plot(plot_x, plot_y)
    plt.xlabel(x_name)
    plt.ylabel("left_PWM")
    
    plt.subplot(VERTICAL_NUMBER, 1, 2)
    plt.scatter(conditions[:, index], actions[:, 1], c="gold")
    plot_x, plot_y = zip(*sorted(zip(conditions[:, index], predictions[:, 1]), key=lambda x: x[0]))
    plt.plot(plot_x, plot_y)
    plt.xlabel(x_name)
    plt.ylabel("right_PWM")

# plot_subplot(1, 2, "selfPosX")
# plot_subplot(2, 3, "selfPosY")
plot_subplot(3, 0, "selfAngle")
plot_subplot(4, 1, "L_sensor")
plot_subplot(4, 2, "L_T_sensor")
plot_subplot(5, 3, "F_sensor")
plot_subplot(6, 4, "R_T_sensor")
plot_subplot(6, 5, "R_sensor")

plt.show()

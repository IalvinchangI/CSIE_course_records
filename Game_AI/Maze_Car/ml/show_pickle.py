import os

import collect
from pprint import pprint
import matplotlib.pyplot as plt
import numpy

FILE_PATH = r".\data"

def format(data: list) -> str:
    output: str = "\t"
    output += f"selfPos: {data[0]['selfPos']}\t"
    output += f"selfAngle: {data[0]['selfAngle']}\t"
    # output += f"sensor1: {data[0]['sensor1']}\t"
    # output += f"sensor2: {data[0]['sensor2']}"
    output += f"sensor2: {data[0]['sensor3']}"

    output += "\t|\t"
    output += f"left: {data[1]['left_PWM']}\t"
    output += f"right: {data[1]['right_PWM']}\t"

    return output

def draw_format(data: numpy.ndarray, label: str, color: str=None) -> None:
    plt.scatter(data[:, 0], data[:, 1], c=color, label=label)

def value2direction(value: tuple) -> tuple[int, str]:
    if value[0] == value[1]:
        if value[0] > 0:
            return 1, "forward"
        else:
            return 3, "backward"
    else:
        if value[0] > value[1]:
            return 2, "turn_right"
        else:
            return 4, "turn_left"
type2color = {
    1: "blue", 
    2: "gold", 
    3: "green", 
    4: "red", 
}


for i, file_name in enumerate(os.listdir(FILE_PATH), 1):
    '''
    print(f"<< {file_name} >>")
    with collect.ML_Data(FILE_PATH, file_name) as file:
        for i, data in enumerate(file.read(), 1):
            # """
            print(f"[{i}] {format(data)}")
            """
            print(f"[{i}]")
            pprint(data)
            # """
    print(f"<< {file_name} >>")
    input("exit? ")
    print("\n")
    '''
    with collect.ML_Data(FILE_PATH, file_name) as file:
        data = numpy.array(list(map(lambda x: numpy.array((x[0]['sensor1'][0], x[0]['sensor1'][2], x[1]['left_PWM'], x[1]['right_PWM'])), file.read())))
        # plt.figure(i)
        draw_format(data[:, 0:2], label=str(i))
        # for dot in data:
        #     draw_format(dot[0:2].reshape(1, -1), label=value2direction(dot[2:])[1], color=type2color[value2direction(dot[2:])[0]])
    # '''
plt.legend()
plt.show()
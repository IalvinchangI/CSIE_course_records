import os
from collect import ML_Data
import numpy

DATA_PATH = r".\data"
STORE_PATH = r".\edit_data"
STORE_NAME = "edit"

def sensor_random(sensor: numpy.ndarray) -> numpy.ndarray:
    shift_2D = numpy.random.uniform(low=-7, high=7, size=(2))  # (^, >)
    sensor += numpy.array((
        sensor[0] - shift_2D[1], 
        sensor[1] + sum(shift_2D) * (2 ** 0.5), 
        sensor[2] + shift_2D[0], 
        sensor[3] + sum(shift_2D) * (2 ** 0.5), 
        sensor[4] + shift_2D[1]
    ))
    for i in range(len(sensor)):
        if sensor[i] < 0:
            sensor[i] = 0
    return numpy.abs(sensor)

for _ in range(50):
    output_data: list = list()
    for file_name in os.listdir(DATA_PATH):
        with ML_Data(DATA_PATH, file_name) as datas:
            for data in datas.read():
                # data[0]["endingPos"] += numpy.random.uniform(low=-10, high=10, size=(2))
                # data[0]["endingPos"] = numpy.zeros((2))
                data[0]["selfPos"] += numpy.random.uniform(low=-5, high=5, size=(2))

                if abs(data[1]["left_PWM"]) == 50:
                    data[0]["selfAngle"] += numpy.random.uniform(low=-5, high=5, size=(1))
                else:
                    data[0]["selfAngle"] += numpy.random.uniform(low=-2, high=2, size=(1))
                
                # data[0]["sensor"] = sensor_random(data[0]["sensor"])
                data[0]["sensor"] = numpy.abs(data[0]["sensor"] + numpy.random.uniform(low=-3, high=3, size=(5)))

                output_data.append(data)
                    
    with ML_Data(STORE_PATH, STORE_NAME + ".pickle", mode="w") as file:
        file.add_data(output_data)
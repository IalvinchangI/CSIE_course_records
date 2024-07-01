from sklearn.tree import DecisionTreeRegressor
from sklearn.multioutput import MultiOutputRegressor

import os
from collect import ML_Data
import numpy

MAX_DEPTH = 40
MIN_SAMPLES_SPLIT = 5

MODEL_NAME = f"tree_{MAX_DEPTH}_{MIN_SAMPLES_SPLIT}_NoPos"
MODEL_PATH = r".\model"

DATA_PATH = r".\edit_data"


# load data
def data_getter(data: list) -> numpy.ndarray:
    # condition
    condition: numpy.ndarray = numpy.hstack((
        # data[0]['endingPos'], 
        # data[0]['selfPos'], 
        data[0]['selfAngle'], 
        data[0]['sensor']
    ))
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


# model
model = MultiOutputRegressor(DecisionTreeRegressor(max_depth=MAX_DEPTH, min_samples_split=MIN_SAMPLES_SPLIT))

model.fit(conditions, actions)

with ML_Data(MODEL_PATH, MODEL_NAME + ".pickle", mode="w") as file:
    file.add_data(model)


# predict
predictions = model.predict(conditions)
correct = 0
for pre, act in zip(predictions, actions):
    if (pre == act).all():
        correct += 1
print(f"rate = {correct / len(predictions) * 100:10.10}%")

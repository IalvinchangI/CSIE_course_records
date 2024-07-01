# 讀模組用

import os
from ml.APM.collect import ML_Data
# from ml.Qtable import Qtable_model

import numpy
import matplotlib.pyplot as plt

PATH = r".\TankMan\model"
# MODEL1_PATH = os.path.join(PATH, "train2P1")
# MODEL2_PATH = os.path.join(PATH, "train2P2")
# MODEL2_PATH = os.path.join(PATH, "competeP2")
LENGTH = 6

def get_description(path: str) -> tuple:
    output_reward = list()
    output_score = list()
    file_list = os.listdir(path)
    file_list.sort(key=lambda x: int(x.split(".")[0][LENGTH:] if x.split(".")[0][LENGTH:] != "" else "0"))
    for file in file_list:
        with ML_Data(path, file) as model_file:
            description = model_file.read()
            output_reward.append(description["total_reward"])
            output_score.append(description["final_score"])
            model = model_file.read()
    
    return output_reward, output_score, model


# y
model1_reward, model1_score, model1 = get_description(PATH)
# model1_reward, model1_score, model1 = get_description(MODEL1_PATH)
# model2_reward, model2_score, model2 = get_description(MODEL2_PATH)

# x
length = len(model1_reward)
# length = max(len(model1_reward), len(model2_reward))
# length = len(model2_reward)
Round = numpy.arange(0, length, 1)

# plot (x, y)
plt.plot(Round, model1_score, color="k", label="model1_score")
plt.plot(Round, model1_reward, "--", color="k", label="model1_reward")
# plt.plot(Round, model2_score, color="r", label="model2_score")
# plt.plot(Round, model2_reward, "--", color="r", label="model2_reward")

plt.legend()

model1_table: numpy.ndarray = model1._Q_table._get_whole_Qtable()
zero_count1 = numpy.sum(model1_table == 0)
total = model1_table.size
print(f"zero = {zero_count1}\ttotal = {total}\t -> {zero_count1 / total}")

# model2_table: numpy.ndarray = model2._get_whole_Qtable()
# zero_count2 = numpy.sum(model2_table == 0)
# total = model2_table.size
# print(f"zero = {zero_count2}\ttotal = {total}\t -> {zero_count2 / total}")

plt.show()

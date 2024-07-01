# 造模型用的

import os
from ml.collect import ML_Data

from ml.Qtable import Qtable_model


ALPHA = 0.1
GAMMA = 0.9

MODEL1_NAME = f"QT_model1_{int(ALPHA * 10)}_{int(GAMMA * 10)}_"
MODEL2_NAME = f"QT_model2_{int(ALPHA * 10)}_{int(GAMMA * 10)}_"
EXTENSION = ".pickle"

PATH = r".\swimming-squid-battle\model"
MODEL1_PATH = os.path.join(PATH, "train2P1")
MODEL2_PATH = os.path.join(PATH, "train2P2")

# FEATURE_SHAPE = [6, 4, 6, 8, 8, 8, 8]
FEATURE_SHAPE = [8, 8, 8, 8]
ACTION_SHAPE = [4, ]


def build_model(dir_name: str, model_name: str, feature_shape: list, action_shape: list) -> None:
    # description
    description = {
        "name" : model_name, 
        "alpha" : ALPHA, 
        "gamma" : GAMMA, 
        "feature_shape" : feature_shape, 
        "action_shape" : action_shape, 
        "round" : 0, 
        "total_reward" : 0, 
        "final_score" : 0
    }

    # model
    model = Qtable_model(tuple(feature_shape + action_shape), ALPHA, GAMMA)

    with ML_Data(dir_name, model_name + EXTENSION, mode="w") as file:
        file.add_data(description)
        file.add_data(model)



build_model(MODEL1_PATH, MODEL1_NAME, FEATURE_SHAPE, ACTION_SHAPE)
build_model(MODEL2_PATH, MODEL2_NAME, FEATURE_SHAPE, ACTION_SHAPE)

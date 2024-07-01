# 造模型用的

from ml.APM.collect import ML_Data

from ml.APM.robot import Dodger, EnemyShooter



MODEL1_NAME = f"dodger"
# MODEL2_NAME = f"QT_model2_{int(ALPHA * 10)}_{int(GAMMA * 10)}_"
EXTENSION = ".pickle"

PATH = r".\TankMan\model"
# MODEL1_PATH = os.path.join(PATH, "train2P1")
# MODEL2_PATH = os.path.join(PATH, "train2P2")


def build_model(dir_name: str, model_name: str) -> None:
    # description
    description = {
        "name" : model_name, 
        "round" : 0, 
        "total_reward" : 0, 
        "final_score" : 0
    }

    # model
    # model = Dodger()
    model = EnemyShooter()

    with ML_Data(dir_name, model_name + EXTENSION, mode="w") as file:
        file.add_data(description)
        file.add_data(model)



build_model(PATH, MODEL1_NAME)
# build_model(MODEL1_PATH, MODEL1_NAME)
# build_model(MODEL2_PATH, MODEL2_NAME)

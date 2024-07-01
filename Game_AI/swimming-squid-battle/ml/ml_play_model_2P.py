import os
from ml.data_transformer import DataTransformer
from ml.Qtable import Qtable_model
from ml.collect import ML_Data



PATH = r".\model"
# MODEL_PATH = os.path.join(PATH, "competeP2")
# MODEL_PATH = os.path.join(PATH, "train2P2")
MODEL_PATH = os.path.join(PATH, "P2")

MODEL_FILE_NAME = "QT_model2_1_9_100" + ".pickle"

class MLPlay:
    ACTION_CODE2NAME: dict = {
        0 : "UP", 
        1 : "RIGHT",
        2 : "DOWN",
        3 : "LEFT",
        4 : "NONE"
    }

    def __init__(self,*args, **kwargs):
        # data transform
        # game_level = kwargs["game_params"]["level"]
        game_level = args[1]["level"]
        if game_level <= 6:
            self.transformer = DataTransformer(400, 400)
        elif game_level == 7:
            self.transformer = DataTransformer(500, 500)
        elif game_level == 8:
            self.transformer = DataTransformer(650, 600)

        # load model
        with ML_Data(MODEL_PATH, MODEL_FILE_NAME) as file:
            description = file.read()
            print(description)
            self.model: Qtable_model = file.read()
        
        print("Initial ml script")

    def update(self, scene_info: dict, *args, **kwargs):
        """
        Generate the command according to the received scene information
        """
        # print("AI received data from game :", json.dumps(scene_info))
        # print(scene_info)

        # data
        state = self.transformer.transform_data(scene_info)

        # action
        action = self.model.act(*state)

        return [self.ACTION_CODE2NAME[action]]

    def reset(self):
        """
        Reset the status
        """
        self.transformer.clear()
        # print("reset ml script")
        pass

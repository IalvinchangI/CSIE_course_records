import os
from ml.train import Trainer


PATH = r".\model"
MODEL_PATH = os.path.join(PATH, "train2P1")

MODEL_NAME = "QT_model1_1_9_47"

class MLPlay:
    def __init__(self,*args, **kwargs):
        self.trainer = Trainer(MODEL_PATH, MODEL_NAME, args[1]["level"])
        # self.trainer = Trainer(MODEL_PATH, MODEL_NAME, kwargs["game_params"]["level"])
        self.trainer.model.eGreddy = 1
        print("Initial ml script")

    def update(self, scene_info: dict, *args, **kwargs):
        """
        Generate the command according to the received scene information
        """
        # print("AI received data from game :", json.dumps(scene_info))
        # print(scene_info)

        # data
        self.trainer.store_state(scene_info)
        self.trainer.add_self_pos((scene_info["self_x"], scene_info["self_y"]))

        # update
        if scene_info["frame"] >= 1:  # can update Qtable
            # (0 <= frame <= ?)
            self.trainer.update_Qtable(scene_info)

        return [self.trainer.get_action(scene_info)]

    def reset(self):
        """
        Reset the status
        """
        self.trainer.decide()
        self.trainer.clear()
        # print("reset ml script")
        pass

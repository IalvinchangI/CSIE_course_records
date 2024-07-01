from ml.collect import ML_Data
import numpy

MODEL_NAME = "tree_40_5_NoPos"
# MODEL_NAME = "tree_40_5"
MODEL_PATH = r".\model"

class MLPlay:
    SPEED = 1
    def __data_getter(self, scene_info: dict) -> numpy.ndarray:
        output: numpy.ndarray = numpy.array((
            # scene_info["end_x"], 
            # scene_info["end_y"], 
            # scene_info["x"], 
            # scene_info["y"], 
            scene_info["angle"], 
            scene_info["L_sensor"], 
            scene_info["L_T_sensor"], 
            scene_info["F_sensor"], 
            scene_info["R_T_sensor"], 
            scene_info["R_sensor"]
        ))

        return output.reshape(1, -1)
    
    def __init__(self, ai_name,*args,**kwargs):
        self.player_no = ai_name
        self.r_sensor_value = 0
        self.l_sensor_value = 0
        self.f_sensor_value = 0
        self.control_list = {"left_PWM" : 0, "right_PWM" : 0}
        # print("Initial ml script")
        print(kwargs)
        with ML_Data(MODEL_PATH, MODEL_NAME + ".pickle") as file:
            self.__model = file.read()

    def update(self, scene_info: dict, *args, **kwargs):
        """
        Generate the command according to the received scene information
        """
        if scene_info["status"] != "GAME_ALIVE":
            return "RESET"
        
        data = self.__data_getter(scene_info)
        prediction = self.__model.predict(data)[0] * self.SPEED

        self.control_list["left_PWM"] = prediction[0]
        self.control_list["right_PWM"] = prediction[1]

        # print(self.__format(data))

        return self.control_list

    def reset(self):
        """
        Reset the status
        """
        # print("reset ml script")
        pass
    
    def __format(self, data: dict) -> str:
        output: str = f"\t{data}\t"

        output += "\t|\t"
        output += f"left: {self.control_list['left_PWM']}\t"
        output += f"right: {self.control_list['right_PWM']}\t"

        return output

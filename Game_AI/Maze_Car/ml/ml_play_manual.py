import pygame
import copy
import numpy
import ml.collect as collect


class MLPlay:
    store_path = r".\data"
    file_name = r"manual.pickle"

    move_speed = 50
    turn_speed = 1

    store_key_cooldown_time = 15

    def __init__(self, ai_name,*args,**kwargs):
        self.player_no = ai_name
        self.r_sensor_value = 0
        self.l_sensor_value = 0
        self.f_sensor_value = 0
        self.control_list = {"left_PWM": 0, "right_PWM": 0}
        self.store_TF = True
        self.store_key_cooldown = 0
        # print("Initial ml script")
        self.__data = list()

    def update(self, scene_info: dict, keyboard: list = [], *args, **kwargs):
        """
        Generate the command according to the received scene information
        """
        if scene_info["status"] != "GAME_ALIVE":
            if scene_info["status"] == "GAME_PASS":
                self.__store2file()
                pass
            return "RESET"
        
        # play
        if pygame.K_w in keyboard or pygame.K_UP in keyboard:
            self.control_list["left_PWM"] = self.move_speed
            self.control_list["right_PWM"] = self.move_speed
        elif pygame.K_a in keyboard or pygame.K_LEFT in keyboard:
            self.control_list["left_PWM"] = -self.turn_speed
            self.control_list["right_PWM"] = self.turn_speed
        elif pygame.K_d in keyboard or pygame.K_RIGHT in keyboard:
            self.control_list["left_PWM"] = self.turn_speed
            self.control_list["right_PWM"] = -self.turn_speed
        elif pygame.K_s in keyboard or pygame.K_DOWN in keyboard:
            self.control_list["left_PWM"] = -self.move_speed
            self.control_list["right_PWM"] = -self.move_speed
        else:
            self.control_list["left_PWM"] = 0
            self.control_list["right_PWM"] = 0
        
        if pygame.K_0 in keyboard:
            if self.store_key_cooldown == 0:
                self.store_TF = not self.store_TF
                self.store_key_cooldown = self.store_key_cooldown_time
                print(f"store? {self.store_TF}")
            else:
                self.store_key_cooldown -= 1
        else:
            self.store_key_cooldown = 0

        # store data
        if self.store_TF and not (self.control_list["left_PWM"] == 0 and self.control_list["right_PWM"] == 0):
            self.__store_data(scene_info, self.control_list)

        return self.control_list

    def reset(self):
        """
        Reset the status
        """
        # print("reset ml script")
        pass

    def __store_data(self, scene_info: dict, action: dict) -> None:
        data = [
            # condition
            dict(), 
            # action
            copy.deepcopy(action)
        ]

        # condition
        data[0]["endingPos"] = numpy.array((scene_info["end_x"], scene_info["end_y"]))
        data[0]["selfPos"] = numpy.array((scene_info["x"], scene_info["y"]))
        # data[0]["deltaPos"] = data[0]["endingPos"] - data[0]["selfPos"]

        data[0]["selfAngle"] = scene_info["angle"]
        # data[0]["deltaAngle"] = 

        data[0]["sensor"] = numpy.array(
            (scene_info["L_sensor"], scene_info["L_T_sensor"], scene_info["F_sensor"], scene_info["R_T_sensor"], scene_info["R_sensor"])
        )
        

        self.__data.append(data)
    
    def __store2file(self):
        print(len(self.__data))
        with collect.ML_Data(self.store_path, self.file_name, "w") as file:
            file.add_data(self.__data)

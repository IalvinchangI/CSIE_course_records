"""
The template of the main script of the machine learning process
"""
import random
import pygame

import pprint

from src.env import IS_DEBUG


from ml.APM.transformer import DataTransformer
from ml.APM.robot import *
from ml.APM.collect import ML_Data


class MLPlay:
    DODGER_PATH: str = r".\model"
    DODGER_NAME: str = "dodger_30" + ".pickle"
    SHOOTER_PATH: str = r".\model"
    SHOOTER_NAME: str = "shooter_11" + ".pickle"

    def __init__(self, ai_name, *args, **kwargs):
        """
        Constructor

        @param side A string "1P" or "2P" indicates that the `MLPlay` is used by
               which side.
        """
        self.side = ai_name
        print(f"Initial Game {ai_name} ml script")
        self.time = 0
        self.action = "NONE"

        self.init_robot()
        

    def init_robot(self):
        # data
        self.data_transformer: DataTransformer = DataTransformer(
            EnemyShooter.SHOOTING_RADIUS, DataTransformer.SHOOTING_RANGE + 30, 
            1000 // DataTransformer.GRID_STRIDE, 600 // DataTransformer.GRID_STRIDE
        )

        # control, act
        self.controler = Controler()
        self.actor = Actor()

        # load model
        with ML_Data(self.DODGER_PATH, self.DODGER_NAME, mode="r") as dodger_file:
            dodger_description: dict = dodger_file.read()
            dodger_model: Dodger = dodger_file.read()

        with ML_Data(self.SHOOTER_PATH, self.SHOOTER_NAME, mode="r") as shooter_file:
            shooter_description: dict = shooter_file.read()
            shooter_model: EnemyShooter = shooter_file.read()

        self.controler.load_model(
            shooter_model, 
            dodger_model, 
            WallShooter(self.data_transformer), 
            BulletFinder(self.data_transformer), 
            OilFinder(self.data_transformer)
        )


    def run_robot(self, scene_info: dict) -> str:
        # get data
        data: dict = self.data_transformer.get_data(scene_info)
        # print(scene_info["id"], data["pos"])
        # if scene_info["used_frame"] == 1:
        #     pprint.pprint(data["best_bullet_station"])

        # setting
        if self.controler.next_move == -1:
            self.controler.next_move = data["tank_angle"]
        self.actor.update(data["pos"], data["tank_angle"], data["gun_angle"], data["gun_cooldown"])

        # predict
        action: tuple = self.controler.execute(data)

        # command
        command: str = self.actor.translate(*action)
        # print(data["tank_angle"], action[0], data["gun_angle"], action[1], command, sep="\t")

        # setting
        self.data_transformer.update_last_direction(data["tank_angle"])

        return command


    def update(self, scene_info: dict, keyboard=[], *args, **kwargs):
        """
        Generate the command according to the received scene information
        """
        # print(keyboard)
        if scene_info["status"] != "GAME_ALIVE":
            # print(scene_info)
            return "RESET"
        
        if scene_info["used_frame"] % 2 == 0:
            self.action = self.run_robot(scene_info)
            return [self.action]
        return ["NONE"]

        
        """
        if self.side == "1P":
            action = self.run_robot(scene_info)

            if action != "NONE":
                return [action]
            else:
                print(f"{scene_info['used_frame']} : random")
                pass
        move_act = random.randrange(5)
        aim_act = random.randrange(3)
        shoot_cd = random.randrange(15, 31)


        is_shoot = 0
        if scene_info["used_frame"] % shoot_cd == 0:
            is_shoot = random.randrange(2)

        command = []
        if scene_info["used_frame"] == 97:
            command.append("SHOOT")

        if move_act == 1:
            command.append("TURN_RIGHT")
        elif move_act == 2:
            command.append("TURN_LEFT")
        elif move_act == 3:
            command.append("FORWARD")
        elif move_act == 4:
            command.append("BACKWARD")

        if aim_act == 1:
            command.append("AIM_LEFT")
        elif aim_act == 2:
            command.append("AIM_RIGHT")

        if is_shoot and not IS_DEBUG:
            command.append("SHOOT")

        if self.side == "1P":
            if pygame.K_b in keyboard:
                command.append("DEBUG")

        if not command:
            command.append("NONE")
        
        return command
        """

    def reset(self):
        """
        Reset the status
        """
        print(f"reset Game {self.side}")
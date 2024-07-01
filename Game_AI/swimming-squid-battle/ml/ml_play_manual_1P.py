import random
from pprint import pprint

import orjson
import pygame


class MLPlay:
    def __init__(self,ai_name,*args,**kwargs):
        print(kwargs)
        self.round = 1
        print("Initial ml script")

    def update(self, scene_info: dict, keyboard:list=[], *args, **kwargs):
        """
        Generate the command according to the received scene information
        """
        # pprint("AI received data from game :", orjson.dumps(scene_info))
        if scene_info["frame"] == 0:
            # pprint(scene_info)
            pass
        actions = []
        print((scene_info["self_x"], scene_info["self_y"]))

        if pygame.K_UP in keyboard:
            actions.append("UP")
        elif pygame.K_DOWN in keyboard:
            actions.append("DOWN")

        elif pygame.K_LEFT in keyboard:
            actions.append("LEFT")
        elif pygame.K_RIGHT in keyboard:
            actions.append("RIGHT")
        else:
            actions.append("NONE")

        return actions



    def reset(self):
        """
        Reset the status
        """
        print(self.round)
        self.round += 1
        print("reset ml script")
        pass

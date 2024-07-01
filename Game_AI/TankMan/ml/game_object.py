from numpy import ndarray
import numpy
from collections import deque
from itertools import cycle

import random

from ml.APM.transformer import DataTransformer
from ml.train import Trainer
from ml.APM.robot import EnemyShooter






class Bullet():
    COS_45: float = 1 / 2 ** 0.5
    DIRECTION2VELOCITY = {
        0 : numpy.array((0, 8), dtype=float), 
        1 : numpy.array((8, 8), dtype=float) * COS_45, 
        2 : numpy.array((8, 0), dtype=float), 
        3 : numpy.array((8, -8), dtype=float) * COS_45, 
        4 : numpy.array((0, -8), dtype=float), 
        5 : numpy.array((-8, -8), dtype=float) * COS_45, 
        6 : numpy.array((-8, 0), dtype=float), 
        7 : numpy.array((-8, 8), dtype=float) * COS_45, 
    }

    def __init__(self, position: ndarray, direction: int):
        self.position = position
        self.velocity = self.DIRECTION2VELOCITY[direction]
    
    def update(self):
        self.position += self.velocity
    
    def get_next(self) -> ndarray:
        return self.position + self.velocity
    
    def get_pre(self) -> ndarray:
        return self.position - self.velocity
    
    def get_info(self, tank_pos: ndarray) -> list:
        """
        return: [{"direction", "time"}, ...]
        """
        tank_bullet = self.position - tank_pos
        return [{
            "direction" : DataTransformer.get_vector_direction(tank_bullet), 
            "time" : numpy.linalg.norm(tank_bullet) // DataTransformer.BULLET_SPEED  # 最多 10 frame
        }]


class Tank():
    TANK_SIZE = numpy.array((50, 50))

    COS_45: float = 1 / 2 ** 0.5
    DIRECTION2VELOCITY = {
        0 : numpy.array((0, 30), dtype=float), 
        1 : numpy.array((30, 30), dtype=float) * COS_45, 
        2 : numpy.array((30, 0), dtype=float), 
        3 : numpy.array((30, -30), dtype=float) * COS_45, 
        4 : numpy.array((0, -30), dtype=float), 
        5 : numpy.array((-30, -30), dtype=float) * COS_45, 
        6 : numpy.array((-30, 0), dtype=float), 
        7 : numpy.array((-30, 30), dtype=float) * COS_45, 
    }

    def __init__(self, position: ndarray):
        self.position = position.astype(float)
        self.life: int = 3

    def update_tank(self, direction: int):
        self.position += self.DIRECTION2VELOCITY[direction]

    def update_gun(self, direction: int):
        return Bullet(self.position, direction)

    def hit_TF(self, start_pos: ndarray, end_pos: ndarray) -> tuple[bool, float]:
        """ start -> end 是否穿越 tank 以及 差距多遠 """
        radius = numpy.linalg.norm(self.TANK_SIZE) / 2
        center_start = start_pos - ((self.position * 2 + self.TANK_SIZE) / 2)
        center_end = end_pos - ((self.position * 2 + self.TANK_SIZE) / 2)
        end_start = end_pos - start_pos

        distance = numpy.linalg.norm(
            center_start - (numpy.dot(center_start, end_start) / numpy.dot(end_start, end_start)) * end_start
        )
        if distance > radius:
            return False, distance

        if (numpy.all(self.position < start_pos) and numpy.all((self.position + self.TANK_SIZE) > start_pos)) or \
           (numpy.all(self.position < end_pos) and numpy.all((self.position + self.TANK_SIZE) > end_pos)):
            return True, distance

        min_angle = numpy.arctan(radius / numpy.linalg.norm(end_start))
        if numpy.arccos(numpy.dot(center_start, center_end) / numpy.linalg.norm(center_start) / numpy.linalg.norm(center_end)) < min_angle:
            return False, distance
        
        return True, distance
    

    def life_decrease(self) -> bool:
        """ return death or not """
        self.life -= 1
        if self.life <= 0:
            return True
        else:
            return False


    def get_info(self) -> list:
        """
        return: [{"pos", "life"}, ...]
        """
        return [{
            "pos" : self.position, 
            "life" : self.life, 
        }]




class PlayGround():
    WIDTH: int = 1000
    HEIGHT: int = 1000

    def __init__(self, tank: Tank, trainer: Trainer):
        self.tank = tank
        self.bullets: list[Bullet] = list()
        self.trainer = trainer
    
    def run(self, epoch: int, frame: int):
        for _ in range(epoch):
            expect_direction = cycle(tuple(range(0, 8)))
            for f in range(frame):
                if len(self.bullets) == 0:
                    self.generate_bullet()
                
                bullet_info = self.bullets[0].get_info(self.tank.position)[0]
                state = self.trainer.model.state_format(bullet_info["direction"], next(expect_direction), bullet_info["time"])

                # data
                self.trainer.store_state(state)
                
                # update
                action = self.trainer.get_action()
                self.tank.update_tank(action)
                for bullet in self.bullets:
                    bullet.update()
                

                score = 0 if action != expect_direction else 5
                accumulate_score = 0
                for bullet in self.bullets:
                    hit_TF, distance = self.tank.hit_TF(bullet.get_pre(), bullet.position)
                    if hit_TF == True:
                        score -= 10
                    else:
                        accumulate_score += distance


                index = 0
                while index < len(self.bullets):
                    if numpy.linalg.norm(self.bullets[index].position - self.tank.position) > self.BULLET_GENERATE_RADIUS:
                        self.bullets.pop(index)
                        score += 7
                    else:
                        index += 1

                if f >= 1:  # can update Qtable
                    # (0 <= frame <= ?)
                    self.trainer.update_Qtable(score, accumulate_score)
                
            self.trainer.decide()
            self.trainer.clear()

    

    DIRECTION2ANGLE = {
        0 : numpy.pi / 2, 
        1 : numpy.pi / 4, 
        2 : 0, 
        3 : numpy.pi / 4 * 7, 
        4 : numpy.pi / 2 * 3, 
        5 : numpy.pi / 4 * 5, 
        6 : numpy.pi, 
        7 : numpy.pi / 4 * 3, 
    }

    BULLET_GENERATE_RADIUS = DataTransformer.SHOOTING_RANGE + 30
    def generate_bullet(self):
        r, direction = random.randint(60, self.BULLET_GENERATE_RADIUS), random.randint(0, 7)
        pos = numpy.array((r * numpy.cos(self.DIRECTION2ANGLE[direction]), r * numpy.sin(self.DIRECTION2ANGLE[direction])), dtype=float) + self.tank.position
        
        self.bullets.append(Bullet(pos, (direction + 4) % 8))



class PlayGround2():
    WIDTH: int = 1000
    HEIGHT: int = 1000

    def __init__(self, tank: Tank, trainer: Trainer):
        self.my_tank = tank
        self.tanks: list[Tank] = list()
        self.bullets: list[Bullet] = list()
        self.trainer = trainer
    
    def run(self, epoch: int, frame: int):
        for _ in range(epoch):
            expect_direction = cycle(tuple(range(0, 8)))
            cooldown: int = 0
            for f in range(frame):
                if len(self.tanks) == 0:
                    self.generate_tank()
                    
                
                tank_info = self.tanks[0].get_info()[0]
                state = self.trainer.model.state_format(tank_info["pos"] - self.my_tank.position, next(expect_direction))

                # data
                self.trainer.store_state(state)
                
                # update
                action = EnemyShooter.translate_act(self.trainer.get_action())
                if action[0] != -1:
                    self.my_tank.update_tank(action[0])
                elif action[1] != -1 and (cooldown <= 0):
                    self.bullets.append(self.my_tank.update_gun(action[1]))
                
                if cooldown > 0:
                    cooldown -= 1

                for bullet in self.bullets:
                    bullet.update()
                

                score = 0 if action != expect_direction else 1
                accumulate_score = 0
                for tank in self.tanks[:]:
                    for bullet in self.bullets:
                        hit_TF, distance = tank.hit_TF(bullet.get_pre(), bullet.position)
                        if hit_TF == True:
                            die_TF = tank.life_decrease()
                            score += 3
                            if die_TF == True:  # tank remove
                                self.tanks.remove(tank)
                                break


                # tank remove
                index = 0
                while index < len(self.tanks):
                    if numpy.linalg.norm(self.tanks[index].position - self.my_tank.position) > DataTransformer.SHOOTING_RANGE:
                        self.tanks.pop(index)
                        # score -= 1
                    else:
                        index += 1
                

                # bullet remove
                index = 0
                while index < len(self.bullets):
                    if numpy.linalg.norm(self.bullets[index].position - self.my_tank.position) > DataTransformer.SHOOTING_RANGE:
                        self.bullets.pop(index)
                    else:
                        index += 1


                if f >= 1:  # can update Qtable
                    # (0 <= frame <= ?)
                    self.trainer.update_Qtable(score, accumulate_score)
                
            self.trainer.decide()
            self.trainer.clear()

    

    DIRECTION2ANGLE = {
        0 : numpy.pi / 2, 
        1 : numpy.pi / 4, 
        2 : 0, 
        3 : numpy.pi / 4 * 7, 
        4 : numpy.pi / 2 * 3, 
        5 : numpy.pi / 4 * 5, 
        6 : numpy.pi, 
        7 : numpy.pi / 4 * 3, 
    }

    TANK_GENERATE_RADIUS = EnemyShooter.SHOOTING_RADIUS
    def generate_tank(self):
        r, angle = random.randint(20, self.TANK_GENERATE_RADIUS), (random.randint(0, 359) * numpy.pi / 180.)
        pos = numpy.array((r * numpy.cos(angle), r * numpy.sin(angle)), dtype=float) + self.my_tank.position
        
        self.tanks.append(Tank(pos))

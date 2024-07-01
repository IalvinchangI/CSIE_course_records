from numpy import ndarray
import numpy


from ml.APM.transformer import DataTransformer, jStar
from ml.APM.Qtable import Qtable_model





class roadFinder(jStar):
    """ 搜路用的 """

    def __init__(self, dataTransformer: DataTransformer):
        self.data: DataTransformer = dataTransformer

    
    def get_road_direction(self, start_pos: ndarray, end_pos: ndarray) -> int:
        """
        取得接下來要去的方向

        -1 : 原地
        
        0 ~ 7 : 方向
        """
        

        path = self.solve_maze(
            self.data.csr_matrix_ground, 
            len(self.data.ground[0]), 
            self.get_ground_pos(start_pos), 
            self.get_ground_pos(end_pos))
    
        if len(path) >= 3:
            # print(self.get_ground_pos(start_pos), self.get_ground_pos(end_pos), DataTransformer.get_vector_direction(path[2] - path[0]), sep="\t", end="\t")
            return DataTransformer.get_vector_direction(path[2] - path[0])
        elif len(path) == 2:
        # if len(path) >= 2:
            # print(self.get_ground_pos(start_pos), self.get_ground_pos(end_pos), DataTransformer.get_vector_direction(path[1] - path[0]), sep="\t", end="\t")
            return DataTransformer.get_vector_direction(path[1] - path[0])
        # print(self.get_ground_pos(start_pos), self.get_ground_pos(end_pos), -1, sep="\t", end="\t")
        return -1  # 原地

    


class EnemyShooter():
    SHOOTING_RADIUS: int = 250
    POS_SPLIT: int = 16

    def __init__(self):
        """ create modal """
        self.__Q_table = Qtable_model((16, 16, 8, 16))  # enemy_x, enemy_y, expect_moving_direction, action(direction)


    @property
    def _Q_table(self) -> Qtable_model:
        return self.__Q_table
    

    @staticmethod
    def state_format(enemy_self_pos_delta: ndarray, expect_moving_direction: int) -> tuple:
        """ it will help you transform the enemy-self posision delta """
        enemy_x, enemy_y = EnemyShooter.transform(enemy_self_pos_delta)
        return (enemy_x, enemy_y, expect_moving_direction)
    

    @staticmethod
    def transform(enemy_self_pos_delta: ndarray) -> ndarray:
        return numpy.floor(numpy.clip(
            (enemy_self_pos_delta.astype(int) + EnemyShooter.SHOOTING_RADIUS) * (EnemyShooter.POS_SPLIT / EnemyShooter.SHOOTING_RADIUS / 2), 
            0, 
            EnemyShooter.POS_SPLIT - 1
        )).astype(int)
    

    def update(self, state: tuple, action: int, reward: float, new_state: tuple):
        """
        會在下一frame被call
        state: 上一frame的
        action: 上一frame的
        reward: 從上一frame和這一frame可推知
        next_max_reward: 從這一frame和上一frame可推知
        """
        self.__Q_table.update(state, action, reward, new_state)


    def train_act(self, enemy_x: int, enemy_y: int, expect_moving_direction: int) -> int:
        """ 取得模型給出的方向，訓練用 """
        return self.__Q_table.get_action(enemy_x, enemy_y, expect_moving_direction)


    def act(self, enemy_x: int, enemy_y: int, expect_moving_direction: int) -> tuple[int]:
        """ 取得模型給出的最佳方向 """
        return self.translate_act(self.__Q_table.act(enemy_x, enemy_y, expect_moving_direction))


    @staticmethod
    def translate_act(act: int) -> tuple[int]:
        """ 16 -> (8, 8) """
        if act // 8 == 0:  # tank
            return (act % 8, -1)

        if act // 8 == 1:  # gun
            return (-1, act % 8)




class Dodger():
    def __init__(self):
        """ create modal """
        self.__Q_table = Qtable_model((8, 8, 3, 8))  # bullet_direction, expect_moving_direction, time, action(direction)
    
    
    @property
    def _Q_table(self) -> Qtable_model:
        return self.__Q_table


    @staticmethod
    def time_calculate(time: int) -> int:
        # time = 0 ~ 8
        time = min(time, 8)
        time = max(0, time)

        return int(time) // 3
    
    @staticmethod
    def state_format(bullet_direction: int, expect_moving_direction: int, time: int) -> tuple:
        return (bullet_direction, expect_moving_direction, Dodger.time_calculate(time))


    def update(self, state: tuple, action: int, reward: float, new_state: tuple):
        """
        會在下一frame被call
        state: 上一frame的
        action: 上一frame的
        reward: 從上一frame和這一frame可推知
        next_max_reward: 從這一frame和上一frame可推知
        """
        self.__Q_table.update(state, action, reward, new_state)


    def train_act(self, bullet_direction: int, expect_moving_direction: int, time: int) -> int:
        """ 取得模型給出的方向，訓練用 """
        return self.__Q_table.get_action(bullet_direction, expect_moving_direction, self.time_calculate(time))


    def act(self, bullet_direction: int, expect_moving_direction: int, time: int) -> int:
        """ 取得模型給出的最佳方向 """
        return self.__Q_table.act(bullet_direction, expect_moving_direction, self.time_calculate(time))
    


class WallShooter():
    def __init__(self, dataTransformer: DataTransformer) -> None:
        self.data: DataTransformer = dataTransformer
    

    def __find_wall(self, walls: list[ndarray], my_pos: ndarray) -> ndarray:
        for wall in walls:
            if numpy.linalg.norm(wall - my_pos) >= DataTransformer.SHOOTING_RANGE:  # out of range
                continue
            if self.data.hit_team_TF(wall) == False:
                return wall
        return None


    def act(self, walls: list, my_pos: ndarray) -> tuple[int]:
        wall = self.__find_wall(walls, my_pos)
        if wall is None:
            return (-1, -1)
        
        wall_self = wall - my_pos
        distance = numpy.linalg.norm(wall_self)
        direction = DataTransformer.get_vector_direction(wall_self)
        if distance < 100:
            return (-1, direction)  # move gun
        return (direction, -1)  # move tank



class BulletFinder(roadFinder):
    def act(self, tank_pos: ndarray, bullet_station_pos: ndarray) -> int:
        return self.get_road_direction(tank_pos, bullet_station_pos)


class OilFinder(roadFinder):
    def act(self, tank_pos: ndarray, oil_station_pos: ndarray) -> int:
        return self.get_road_direction(tank_pos, oil_station_pos)
    




class Controler():
    BULLET_MINIMUM: int = 2
    """ 必須找彈藥線 """
    OIL_MAXIMUM: int = 50  # no usage
    """ 最早找油線 """
    OIL_MINIMUM: int = 40
    """ 必須找油線 """


    def __init__(self):
        self.next_move: int = -1
        """
        direction
        -1 : 沒值
        """


    def load_model(self,
                   enemy_shooter: EnemyShooter, 
                   dodger: Dodger, 
                   wall_shooter: WallShooter, 
                   bullet_finder: BulletFinder, 
                   oil_finder: OilFinder, 
    ):
       self.enemy_shooter: EnemyShooter = enemy_shooter
       self.dodger: Dodger = dodger
       self.wall_shooter: WallShooter = wall_shooter
       self.bullet_finder: BulletFinder = bullet_finder
       self.oil_finder: OilFinder = oil_finder


    def execute(self, data: dict) -> tuple[int]:
        """
        data : DataTransformer.get_data 的 return

        return : (tank_action: int, gun_action: int)
        (-1 : 不動)
        """
        result: list[tuple[int]] = list()

        # 必須找油
        if self.OIL_MINIMUM >= data["oil_have"]:
            # print("oil")
            return (self.oil_finder.act(data["pos"], data["best_oil_station"]["pos"]), -1)
        

        # dodge
        if len(data["bullet_attack"]) != 0:
            bullets = sorted(data["bullet_attack"], key=lambda x: x["time"])  # small -> big
            predict_direction = self.next_move  # iterate
            for bullet in bullets:
                if predict_direction == -1:
                    predict_direction = 0                                                            # TODO 自定義 predict_direction
                predict_direction = self.dodger.act(bullet["direction"], predict_direction, bullet["time"])
            result.append((predict_direction, -1))
            # print("dodge")
        

        # bullet_finder
        if self.BULLET_MINIMUM >= data["bullet_have"]:
            result.append((self.bullet_finder.act(data["pos"], data["best_bullet_station"]["pos"]), -1))
            # print("bullet_finder")
        

        # enemy_shooter
        if len(data["enemy"]) != 0 and data["life"] > 1:
            enemies = sorted(data["enemy"], key=lambda x: x["distance"])  # small -> big
            predict_direction = self.next_move  # iterate
            for enemy in enemies:
                if predict_direction == -1:
                    if self.next_move == -1:
                        predict_direction = 0                                                            # TODO 自定義 predict_direction
                    else:
                        predict_direction = self.next_move
                action = self.enemy_shooter.act(*EnemyShooter.state_format(enemy["pos"], predict_direction))
                predict_direction = action[0]
            result.append((predict_direction, action[1]))
            # print("enemy_shooter")


        # wall_shooter
        if len(data["walls"]) != 0:
            result.append(self.wall_shooter.act(data["walls"], data["pos"]))
            # print("wall_shooter")

        
        # print(result)
        # check action
        if len(result) == 0:
            return (-1, -1)
        elif len(result) == 1:
            self.next_move = -1
            return result[0][0], result[0][1]
        else:
            for i in range(1, len(result)):
                if result[i][0] != -1:
                    self.next_move = result[i][0]
                    break
            else:
                self.next_move = -1
            return result[0][0], result[0][1]










class Actor():
    def __init__(self):
        self.pos: ndarray = None
        self.tank_angle: int = None
        self.gun_angle: int = None
        self.gun_cooldown: int = None
    

    def update(self, pos: ndarray, tank_angle: int, gun_angle: int, gun_cooldown: int):
        self.pos: ndarray = pos
        self.tank_angle: int = tank_angle
        self.gun_angle: int = gun_angle
        self.gun_cooldown: int = gun_cooldown


    def translate(self, tank_action: int, gun_action: int) -> str:
        """
        優先 tank_action

        -1 : 不動
        """
        # print("\t", self.tank_angle, tank_action, sep="\t")
        if tank_action != -1:
            if tank_action == self.tank_angle:
                return "FORWARD"
            
            tank_action_half = tank_action % 4
            tank_angle_half = self.tank_angle % 4
            if tank_action_half == tank_angle_half:
                return "BACKWARD"
            
            return "TURN_RIGHT" if self.min_turns_with_direction(tank_angle_half, tank_action_half, 4) == 1 else "TURN_LEFT"
        
        if gun_action != -1:
            if gun_action == self.gun_angle:
                return "SHOOT"
            
            return "AIM_RIGHT" if self.min_turns_with_direction(self.gun_angle, gun_action, 8) == 1 else "AIM_LEFT"
        
        return "NONE"


    def min_turns_with_direction(self, current_angle: int, target_angle: int, total_angle: int) -> int:
        """
        clockwise : 1
        counterclockwise : 0
        """
        # clockwise and counterclockwise minimal angle
        clockwise_turns = (target_angle - current_angle) % total_angle
        counterclockwise_turns = (current_angle - target_angle) % total_angle
        
        # minimal angle
        if clockwise_turns <= counterclockwise_turns:  # clockwise
            return 1
        else:                                          # counterclockwise
            return 0






# 資料整理用

import numpy


class DataTransformer():
    """
    屬性:
    1. state
    2. scores  # (food + wall)
    3. self_precise_pos
    4. world_width
    5. world_height
    6. grid  # 每個格子的長寬 -> pos * grid = 在grid上的座標

    可以存: 
    1. 上一frame的 我方分數

    常數:
    1. x, y 的判定間格
    2. 4個方向上的分數的 range
    3. 與食物的距離 和 分數要乘上的倍數
    4. 牆壁的分數
    5. 分割兩個區塊的分割線的方程式
    
    數學:
    1. 分數轉換

    方法:
    1. get_delta_score  # 得到上一frame的reward
    2. transform_data  # 轉換資料用
    3. clear  # 每一回合結束要清一次
    """

    GRID_COUNT: int = 25
    FOOD_SCORE_RANGE = (7, 0)
    DISTANCE_AND_RATE = (
        ( 70, 5.0), 
        (120, 2.0), 
        (240, 1.0), 
        (400, 0.5)
    )
    WALL_DISTANCE = 50
    WALL_SCORE = -8
    OPPONENT_SCORE = 8
    BOUNDARY_LINE = numpy.array((  # (BOUNDARY_LINE) dot (位置差)
        (1,  1), 
        (1, -1)
    ))
    def score2level(self, x: numpy.ndarray) -> numpy.ndarray:
        return numpy.floor(numpy.clip(
            ((self.FOOD_SCORE_RANGE[0] - self.FOOD_SCORE_RANGE[1] + 1) / (1 + numpy.exp(-0.09 * (x + 5)))) + self.FOOD_SCORE_RANGE[1], 
            self.FOOD_SCORE_RANGE[1], self.FOOD_SCORE_RANGE[0]
        )).astype(int)

    def __init__(self, world_width, world_height) -> None:
        self.__state: tuple = None
        self.__scores: tuple = None
        self.__self_precise_pos: numpy.ndarray = None
        self.__world_width: int = world_width
        self.__world_height: int = world_height
        self.__grid: numpy.ndarray = numpy.array((self.GRID_COUNT / world_width, self.GRID_COUNT / world_height))  # (x, y)

        self.__last_my_Scores: int = 0
    
    def get_delta_score(self, scene_info: dict) -> tuple:
        """ 得到上一frame的reward """
        # 取出 last_my_Scores 後放入新的
        output = scene_info["score"] - self.__last_my_Scores
        self.__last_my_Scores = scene_info["score"]
        return output

    def transform_data(self, scene_info: dict) -> tuple:
        """
        OUTPUT DATA:
        1. self_x % GRID_COUNT : 就位置  X
        2. self_y % GRID_COUNT : 就位置  X
        3. self_lv : 判斷速度、大小用的
        4. (opponent_x - self_x) % GRID_COUNT : 就位置差 : ?  可合併
        5. (opponent_y - self_y) % GRID_COUNT : 就位置差 : ?
        6. opponent_lv : 判斷速度、大小用的 : ?
        7. 4個方向上的食物分數高低 + 4個方向上的牆壁距離
        """
        # 處理資料
        output = list()
        self.__self_precise_pos = numpy.array((scene_info["self_x"], scene_info["self_y"]))
        # output += list(self.__get_grid_position(self.__self_precise_pos))  # 1, 2
        # output.append(scene_info["self_lv"] - 1)  # 3
        # output += list(self.__get_grid_position(numpy.array((scene_info["opponent_x"], scene_info["opponent_y"])) - self.__self_precise_pos))  # 4, 5
        # output.append(self.__get_direction(numpy.array((scene_info["opponent_x"], scene_info["opponent_y"]))))  # 4, 5 合併
        # output.append(scene_info["opponent_lv"] - 1)  # 6
        # 7 : (4, )
        foods_score = self.__get_foods_score(scene_info["foods"])
        wall_score = self.__close_to_wall()
        opponent_score = self.__get_opponent_score(numpy.array((scene_info["opponent_x"], scene_info["opponent_y"])), 
                                                   scene_info["self_lv"] - scene_info["opponent_lv"]
        )
        self.__scores = list(self.score2level(foods_score + wall_score + opponent_score))
        # print(f"{self.__scores}")
        output += self.__scores
        
        # 把資料存入 States
        self.__state = tuple(output)
        return self.__state

    def __get_grid_position(self, pos: numpy.ndarray) -> numpy.ndarray:
        output = numpy.floor(pos / self.__grid)
        outside_index = numpy.where(output >= self.GRID_COUNT)
        for i in outside_index:
            output[i] = self.GRID_COUNT - 1
        return output

    def __get_direction(self, pos: numpy.ndarray) -> int:
        where = numpy.dot(self.BOUNDARY_LINE, (pos - self.__self_precise_pos))
        if where[0] <  0 and where[1] >= 0: return 0  # up
        if where[0] >= 0 and where[1] >= 0: return 1  # right
        if where[0] >= 0 and where[1] <  0: return 2  # down
        if where[0] <  0 and where[1] <  0: return 3  # left

    def __get_foods_score(self, foods: list) -> numpy.ndarray:
        output = numpy.zeros((4, ))
        for food in foods:
            direction, score = self.__food_direction_score(food)  # 得到每個食物的方向、分數
            output[direction] += score  # 把分數加到對應的方向裡
        return output
    
    def __food_direction_score(self, food: dict) -> tuple:
        score = 0
        pos = numpy.array((food["x"], food["y"]))

        # 根據距離，給予不同的分數比例
        distance = numpy.linalg.norm(pos - self.__self_precise_pos)
        score = food["score"] * self.__get_distance_rate(distance)

        return self.__get_direction(pos), score

    def __get_distance_rate(self, distance: float) -> float:
        """ 根據距離，給予不同的分數比例 """
        for pair in self.DISTANCE_AND_RATE:
            if distance <= pair[0]:
                return pair[1]
        return 0
    
    def __close_to_wall(self) -> numpy.ndarray:
        output = numpy.zeros((4, ))
        output[0] = self.WALL_SCORE * self.__get_distance_rate(self.__self_precise_pos[1])  # up
        output[1] = self.WALL_SCORE * self.__get_distance_rate(self.__world_width  - self.__self_precise_pos[0])  # left
        output[2] = self.WALL_SCORE * self.__get_distance_rate(self.__world_height - self.__self_precise_pos[1])  # down
        output[3] = self.WALL_SCORE * self.__get_distance_rate(self.__self_precise_pos[0])  # right
        # if self.__self_precise_pos[1] < self.WALL_DISTANCE: output[0] = self.WALL_SCORE  # up
        # if self.__self_precise_pos[0] > (self.__world_width  - self.WALL_DISTANCE): output[1] = self.WALL_SCORE  # left
        # if self.__self_precise_pos[1] > (self.__world_height - self.WALL_DISTANCE): output[2] = self.WALL_SCORE  # down
        # if self.__self_precise_pos[0] < self.WALL_DISTANCE: output[3] = self.WALL_SCORE  # right
        return output

    def __get_opponent_score(self, pos: numpy.ndarray, level_delta: int) -> numpy.ndarray:
        """
        level_delta > 0 : 加分
        level_delta < 0 : 扣分
        """
        output = numpy.zeros((4, ))
        output[self.__get_direction(pos)] = self.OPPONENT_SCORE * self.__get_distance_rate(numpy.linalg.norm(pos - self.__self_precise_pos)) * level_delta - 1
        return output

    def clear(self) -> None:
        self.__state = None
        self.__scores = None
        self.__self_precise_pos = None

        self.__last_my_Scores = 0

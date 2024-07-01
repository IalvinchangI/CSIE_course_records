from numpy import ndarray
import numpy

# find road
from scipy.sparse import csr_matrix
from scipy.sparse.csgraph import shortest_path

import pprint





class DataTransformer():
    MIDDLE_GROUND: int = 500
    GRID_STRIDE: int = 25
    """ 單位 : px """
    SHOOTING_RANGE: int = 300
    """ 單位 : px """
    TANK_SIZE: ndarray = numpy.array((25, 25))
    """ 單位 : px """
    TANK_SIZE_TURN: ndarray = numpy.array((35, 35))
    """ 單位 : px """
    TANK_TURN_SHIFT: ndarray = numpy.array((5, 5))
    """ 單位 : px """
    WALL_SIZE: ndarray = numpy.array((25, 25))
    """ 單位 : px """
    BULLET_SPEED: int = 30
    """ 單位 : px / frame """

    def __init__(self, detect_radius: float, danger_zone_radius: float, ground_width: int, ground_height: int):
        """
        detect_radius : 偵測敵人的半徑 (單位 : px)
        danger_zone_radius : 偵測子彈的半徑 (單位 : px)
        """
        self.detect_radius: float = detect_radius
        """ 偵測敵人的半徑 (單位 : px) """
        self.danger_zone_radius: float = danger_zone_radius
        """ 偵測子彈的半徑 (單位 : px) """
        self.ground: ndarray = numpy.zeros((ground_height, ground_width))
        """ 地圖 """
        self.csr_matrix_ground: csr_matrix = None
        """ csr_matrix 地圖 """
        self.walls: list[ndarray] = list()
        """ 所有牆壁 """
        self.pre_bullets: dict = dict()
        """ 上一幀的子彈 """
        self.side: int = -1
        """ team """
        self.tank_pos: ndarray = None
        """ tank 座標 """
        self.last_direction: int = -1
        """ 上一個方向 """
        self.team_member: list = []
        """ 隊友資訊 """
    

    @staticmethod
    def get_tank_real_pos(pos: ndarray, tank_angle: int) -> ndarray:
        if tank_angle % 2 == 0:
            return pos
        return pos + DataTransformer.TANK_TURN_SHIFT


    OFFSET: float = numpy.cos(numpy.pi / 8)
    COS_45: float = 1 / 2 ** 0.5
    VECTOR0: ndarray = numpy.array((0, -1))
    VECTOR1: ndarray = numpy.array((COS_45, -COS_45))
    VECTOR2: ndarray = numpy.array((1, 0))
    VECTOR3: ndarray = numpy.array((COS_45, COS_45))

    @staticmethod
    def get_dot(vector: ndarray, unit_vector: ndarray) -> float:
        vector_length = numpy.linalg.norm(vector)
        if vector_length == 0:
            return 0
        return numpy.dot(vector, unit_vector) / vector_length

    @staticmethod
    def get_unit_vector(vector: ndarray) -> ndarray:
        vector_length = numpy.linalg.norm(vector)
        if vector_length == 0:
            return numpy.zeros_like(vector)
        return vector / vector_length

    @staticmethod
    def get_vector_direction(vector: ndarray) -> int:
        dot = DataTransformer.get_dot(vector, DataTransformer.VECTOR0)
        if dot >= DataTransformer.OFFSET:
            return 0
        elif dot <= -DataTransformer.OFFSET:
            return 4
        
        dot = DataTransformer.get_dot(vector, DataTransformer.VECTOR1)
        if dot >= DataTransformer.OFFSET:
            return 1
        elif dot <= -DataTransformer.OFFSET:
            return 5
        
        dot = DataTransformer.get_dot(vector, DataTransformer.VECTOR2)
        if dot >= DataTransformer.OFFSET:
            return 2
        elif dot <= -DataTransformer.OFFSET:
            return 6
        
        dot = DataTransformer.get_dot(vector, DataTransformer.VECTOR3)
        if dot >= DataTransformer.OFFSET:
            return 3
        elif dot <= -DataTransformer.OFFSET:
            return 7
        
        return -1
    

    @staticmethod
    def solve_linear(exp1: ndarray, exp2: ndarray) -> ndarray:
        coeff = numpy.array(numpy.array((exp1[:-1], exp2[:-1])))
        const = -numpy.array((exp1[-1], exp2[-1]))
        return numpy.linalg.solve(coeff, const)

    @staticmethod
    def in_block_TF(block_pos: ndarray, block_size: ndarray, position: ndarray, velocity: ndarray) -> bool:
        """
        check whether the expression across the brick
        
        position, velocity: 子彈的
        """
        if velocity[0] == 0:
            m = float("inf")
        else:
            m = velocity[1] / velocity[0]
        expression = numpy.array((m, -1, numpy.dot(position, (-m, 1))))

        up_left = numpy.concatenate((block_pos, [1]))
        down_right = numpy.concatenate((block_pos + block_size, [1]))
        up_right = numpy.concatenate((block_pos + numpy.array((block_size[0], 0)), [1]))
        down_left = numpy.concatenate((block_pos + numpy.array((0, block_size[1])), [1]))

        # check whether diagonal point are at the same side of the expression
        value1 = numpy.dot(up_left, expression) * numpy.dot(down_right, expression)
        value2 = numpy.dot(up_right, expression) * numpy.dot(down_left, expression)

        if value1 > 0 and value2 > 0:  # at the same side of the expression(line)
            return False
        elif value1 == 0:  # on the line
            return True
        elif value2 == 0:  # on the line
            return True
        else:  # at the different side
            return True


    def __get_danger_bullets(self, scene_info: dict) -> list:
        """
        取得對自己有危險的子彈

        return: [{"id", "direction", "time"}, ...]
        """
        bullets = dict()
        # get bullet
        for bullet in scene_info["bullets_info"]:
            pos = numpy.array((bullet["x"], bullet["y"]))

            if numpy.linalg.norm(pos - self.tank_pos) < self.danger_zone_radius:
                bullets[bullet["id"]] = pos
        
        # check bullet
        output = list()
        if len(self.pre_bullets) != 0:
            for Id, pos in bullets.items():
                if Id not in self.pre_bullets:
                    continue

                velocity = pos - self.pre_bullets[Id]
                bullet_tank = self.tank_pos - pos
                if numpy.dot(bullet_tank, velocity) >= 0:  # check directoin
                    # print("same direction")                                                                               # TODO
                    if self.in_block_TF(self.tank_pos, DataTransformer.TANK_SIZE, pos, velocity):  # hit tank
                        # 子彈的velocity 指向 tank
                        for wall in self.walls:
                            if self.in_block_TF(wall, DataTransformer.WALL_SIZE, pos, velocity):  # hit wall
                                if numpy.linalg.norm(bullet_tank) > numpy.linalg.norm(wall - pos):  # wall is between bullet and tank
                                    # print("hit wall")                                                                               # TODO
                                    break
                        else:  # will hit tank
                            output.append({
                                "id" : Id[:2], 
                                "direction" : self.get_vector_direction(bullet_tank), 
                                "time" : numpy.linalg.norm(bullet_tank) // self.BULLET_SPEED  # 最多 10 frame
                            })
            
        self.pre_bullets = bullets
        return output
        
    def __get_target_enemies(self, scene_info: dict) -> list:
        """
        取得範圍內的敵人

        return: [{"pos" : (pos), "distance", "lives", "gun_angle"}, ...]
        """
        output = list()
        for enemy in scene_info["competitor_info"]:
            enemy_pos = numpy.array((enemy["x"], enemy["y"]))

            enemy_self = enemy_pos - self.tank_pos
            distance = numpy.linalg.norm(enemy_self)
            if distance < self.detect_radius:  # we should care about
                if self.hit_team_TF(enemy_pos) == True:  # will hit team
                    continue
                for wall in self.walls:
                    if self.in_block_TF(wall, DataTransformer.WALL_SIZE, self.tank_pos, enemy_self):  # has wall
                        if distance > numpy.linalg.norm(wall - enemy_pos):  # wall is between tank and tank
                            break
                else:  # will see tank
                    output.append({
                        "pos" : enemy_pos, 
                        "distance" : distance, 
                        "lives" : enemy["lives"], 
                        "gun_angle" : self.__get_gun_angle(enemy["gun_angle"]), 
                    })
        
        return output

    def __get_order_walls(self, scene_info: dict) -> list:
        """
        各牆壁的位置，由弱到強，由近到遠

        return: [(pos), ...]
        """
        walls: list = [{
            "pos": numpy.array((wall["x"], wall["y"])), 
            "norm": numpy.linalg.norm(numpy.array((wall["x"], wall["y"]) - self.tank_pos)), 
            "lives": wall["lives"]
        } for wall in scene_info["walls_info"]]
        walls.sort(key=lambda x: x["norm"])
        walls.sort(key=lambda x: x["lives"])
        
        self.walls = [x["pos"] for x in walls]
        return self.walls

    def __get_team_info(self, scene_info: dict) -> list:
        """
        取得隊友資訊

        return: [{"pos" : (pos), "bullet_have", "oil", "tank_angle"}, ...]
        """
        return [{
            "pos" : numpy.array((tank["x"], tank["y"])), 
            "bullet_have" : tank["power"], 
            "oil" : tank["oil"], 
            "tank_angle" : self.__get_tank_angle(tank["angle"]), 
        } for tank in scene_info["teammate_info"] if tank["id"] != scene_info["id"]]

    def __get_best_bullet_station(self, scene_info: dict) -> dict:
        """
        return: {"pos" : (pos), "distance"}
        """
        # self_ground_pos: ndarray = jStar.get_ground_pos(numpy.array((scene_info["x"], scene_info["y"])))
        
        bullet_stations: list = list()
        for station in scene_info["bullet_stations_info"]:
            pos = numpy.array((station["x"], station["y"]))
            if self.side != -1:
                if (self.side == 0 and pos[0] < self.MIDDLE_GROUND) or (self.side == 1 and pos[0] >= self.MIDDLE_GROUND):
                    continue
            # if len(jStar.solve_maze(self.csr_matrix_ground, len(self.ground[0]), self_ground_pos, jStar.get_ground_pos(pos))) == 0:
            #     continue
            bullet_stations.append({
                "pos" : pos, 
                "distance" : numpy.linalg.norm(numpy.array((station["x"] - scene_info["x"], station["y"] - scene_info["y"]))), 
                # TODO 加入其他人的距離
            })
        bullet_stations.sort(key=lambda x: x["distance"])
        return bullet_stations[0]

    def __get_best_oil_station(self, scene_info: dict) -> dict:
        """
        return: {"pos" : (pos), "distance"}
        """
        # self_ground_pos: ndarray = jStar.get_ground_pos(numpy.array((scene_info["x"], scene_info["y"])))
        oil_stations: list = list()
        for station in scene_info["oil_stations_info"]:
            pos = numpy.array((station["x"], station["y"]))
            if self.side != -1:
                if (self.side == 0 and pos[0] < self.MIDDLE_GROUND) or (self.side == 1 and pos[0] >= self.MIDDLE_GROUND):
                    continue
            # if len(jStar.solve_maze(self.csr_matrix_ground, len(self.ground[0]), self_ground_pos, jStar.get_ground_pos(pos))) != 0:
            #     continue
            oil_stations.append({
                "pos" : pos, 
                "distance" : numpy.linalg.norm(numpy.array((station["x"] - scene_info["x"], station["y"] - scene_info["y"]))), 
                # TODO 加入其他人的距離
            })
        oil_stations.sort(key=lambda x: x["distance"])
        return oil_stations[0]

    ANGLE2DIRECTION = {
        -45 : 7, 
        0 : 6, 
        45 : 5, 
        90 : 4, 
        135 : 3, 
        180 : 2, 
        225 : 1, 
        270 : 0, 
        315 : 7, 
        360 : 6, 
    }
    """ 轉動角度 : 方向 """

    def __get_tank_angle(self, angle: int) -> int:
        """
        return:
        0 : 
        1 : 
        2 : 
        3 : 
        4 : 
        5 : 
        6 : 
        7 : 
        """
        return self.ANGLE2DIRECTION[angle]

    def __get_gun_angle(self, angle: int) -> int:
        """
        return:
        0 : 
        1 : 
        2 : 
        3 : 
        4 : 
        5 : 
        6 : 
        7 : 
        """
        return self.ANGLE2DIRECTION[angle]



    def __build_ground(self, walls: list[ndarray]) -> None:
        self.ground = numpy.zeros_like(self.ground)
        for wall in walls:
            self.ground[wall[1] // self.GRID_STRIDE][wall[0] // self.GRID_STRIDE] = 1
        
        """
        temp = 0
        put = 0
        for y in range(self.ground.shape[0]):
            for x in range(self.ground.shape[1]):
                temp = self.ground[y][x]
                if put == 1:
                    self.ground[y][x] = 1

                if temp == 1:
                    put = 1
                else:
                    put = 0
        put = 0
        for y in range(self.ground.shape[0]):
            for x in range(self.ground.shape[1] - 1, -1, -1):
                temp = self.ground[y][x]
                if put == 1:
                    self.ground[y][x] = 1

                if temp == 1:
                    put = 1
                else:
                    put = 0
        put = 0
        for x in range(self.ground.shape[1]):
            for y in range(self.ground.shape[0]):
                temp = self.ground[y][x]
                if put == 1:
                    self.ground[y][x] = 1

                if temp == 1:
                    put = 1
                else:
                    put = 0
        put = 0
        for x in range(self.ground.shape[1]):
            for y in range(self.ground.shape[0] - 1, -1, -1):
                temp = self.ground[y][x]
                if put == 1:
                    self.ground[y][x] = 1

                if temp == 1:
                    put = 1
                else:
                    put = 0
        """
                    
        # for y in range(len(self.ground)):
        #     for x in range(len(self.ground[y])):
        #         print("　" if self.ground[y][x] == 0 else "██", end="")
        #     print()
        # print()
        
        self.csr_matrix_ground: csr_matrix = jStar.maze_to_graph(self.ground)


    def update_last_direction(self, value: int):
        self.last_direction = value


    def hit_team_TF(self, target_pos: ndarray) -> bool:
        """ 是否打中隊友 """
        target_self = target_pos - self.tank_pos
        distance = numpy.linalg.norm(target_self)
        for member in self.team_member:
            if DataTransformer.in_block_TF(member["pos"], DataTransformer.TANK_SIZE_TURN, self.tank_pos, target_self):  # hit team?
                if numpy.dot(target_self, member["pos"] - self.tank_pos) < 0:
                    continue
                if distance < numpy.linalg.norm(member["pos"] - self.tank_pos):  # target is between tank and tank
                    continue
                return True  # will hit team member
        return False


    def get_data(self, scene_info: dict) -> dict:
        """ return 控制 robot 所需要的資料 """
        # reset
        

        # get data
        tank_angle = self.__get_tank_angle(scene_info["angle"])
        if self.last_direction == -1:
            self.last_direction = tank_angle
        self.tank_pos = DataTransformer.get_tank_real_pos(numpy.array((scene_info["x"], scene_info["y"])), self.last_direction)

        walls = self.__get_order_walls(scene_info)
        self.__build_ground(walls)

        self.team_member = self.__get_team_info(scene_info)

        side: int = 0 if int(scene_info["id"][0]) <= 3 else 1
        self.side = side

        output: dict =  {
            "walls" : walls, 
            "bullet_attack" : self.__get_danger_bullets(scene_info), 
            "enemy" : self.__get_target_enemies(scene_info), 
            "team" : self.team_member, 
            "bullet_have" : scene_info["power"], 
            "best_bullet_station" : self.__get_best_bullet_station(scene_info), 
            "oil_have" : scene_info["oil"], 
            "best_oil_station" : self.__get_best_oil_station(scene_info), 
            "pos" : self.tank_pos, 
            "life" : scene_info["lives"], 
            "tank_angle" : tank_angle, 
            "gun_angle" : self.__get_gun_angle(scene_info["gun_angle"]), 
            "gun_cooldown" : scene_info["cooldown"], 
        }
        return output
    





class jStar():
    POS_SHIFT: ndarray = numpy.array((5, 5))
    """ 把 tank 座標偏移，以防邊界誤判 """

    @classmethod
    def maze_to_graph(cls, maze: ndarray) -> csr_matrix:
        """ 把 2維list 轉成 graph """
        rows, cols = maze.shape
        graph = numpy.zeros((rows * cols, rows * cols))

        def node_index(x, y):
            return y * cols + x

        for y in range(rows):
            for x in range(cols):
                if maze[y][x] == 0:
                    index = node_index(x, y)
                    if y > 0 and maze[y - 1][x] == 0:
                        graph[index, node_index(x, y - 1)] = 1
                    if y < rows - 1 and maze[y + 1][x] == 0:
                        graph[index, node_index(x, y + 1)] = 1
                    if x > 0 and maze[y][x - 1] == 0:
                        graph[index, node_index(x - 1, y)] = 1
                    if x < cols - 1 and maze[y][x + 1] == 0:
                        graph[index, node_index(x + 1, y)] = 1

        return csr_matrix(graph)


    @classmethod
    def solve_maze(cls, graph: ndarray, cols: int, start: ndarray, goal: ndarray) -> list[ndarray]:
        """ 找路 """
        start_index = start[1] * cols + start[0]
        goal_index = goal[1] * cols + goal[0]
        
        dist_matrix, predecessors = shortest_path(csgraph=graph, directed=False, return_predecessors=True)
    
        if numpy.isinf(dist_matrix[start_index, goal_index]):
            return []
        
        path = []
        current = goal_index
        while current != start_index:
            path.append(numpy.array((current % cols, current // cols)))
            current = predecessors[start_index, current]
        
        path.append(start)
        path.reverse()
        return path
        """
        cols = maze.shape[0]
        print(maze.shape)
        graph = self.maze_to_graph(maze)
        
        start_index = start[1] * cols + start[0]
        goal_index = goal[1] * cols + goal[0]
        
        dist_matrix, predecessors = shortest_path(csgraph=graph, directed=False, return_predecessors=True)

        if numpy.isinf(dist_matrix[start_index, goal_index]):
            return []
        
        path = []
        current = goal_index
        while current != start_index:
            path.append(numpy.array((current % cols, current // cols)))
            print(current, path[-1], sep="\t")
            current = predecessors[start_index, current]
        
        path.append(start)
        path.reverse()
        return path
        """


    @classmethod
    def get_ground_pos(cls, pos: ndarray) -> ndarray:
        return ((pos * 2 + DataTransformer.WALL_SIZE) / 2).astype(int) // DataTransformer.GRID_STRIDE


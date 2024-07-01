"""
The template of the main script of the machine learning process
"""

# from sklearn.neighbors import KNeighborsClassifier  # model
import numpy
import time
import sys
import copy

import ml.collect as collect


class MLPlay:
    translate = {
        -1 : "MOVE_LEFT", 
         0 : "NONE", 
         1 : "MOVE_RIGHT"
    }
    platform_width = 40
    platform_y = 400
    platform_edge = 3

    ball_size = 5

    cut_offset = platform_y - 5
    fixed_velocity_offset = platform_y - 10

    def __init__(self,ai_name, *args, **kwargs):
        """
        Constructor
        """
        self.__scene_info = None
        self.__pre_position = None  # previous tick of ball's position (ndarray)
        self.__pre_velocity = None  # previous tick of ball's velocity (ndarray)

        # self.model_brick_1 = collect.ML_Data(r".\model", r"ml_fit_brick-1___.model").read()
        # self.model_brick_2 = collect.ML_Data(r".\model", r"ml_fit_brick-2___.model").read()
        # self.model_boundary = collect.ML_Data(r".\model", r"ml_fit_boundary_ko20_in_120.model").read()
        self.model = collect.ML_Data(r".\model", r"ml_fit_Ox_all.model").read()
        self.name = ai_name
        print(ai_name)

    def update(self, scene_info, *args, **kwargs):
        """
        Generate the command according to the received `scene_info`.
        """

        start = time.time()
        self.__scene_info = scene_info

        """ preprocess """
        # Make the caller to invoke `reset()` for the next round.
        if (scene_info["status"] == "GAME_OVER" or scene_info["status"] == "GAME_PASS"):
            return "RESET"
        
        # play ball
        if not scene_info["ball_served"]:
            self.__start_game()
            return "SERVE_TO_LEFT"
        
        """ play """
        # print(f"{scene_info['frame']:>4}: ", end="\t")
        decide = self.__decision(self.__transform_data_origin())
        condition, platform = self.__transform_data()
        # expression1 = self.model_brick_1.predict(numpy.concatenate(condition).reshape(1, -1))[0]
        # expression2 = self.model_brick_2.predict(numpy.concatenate(condition).reshape(1, -1))[0]
        # command = self.model_boundary.predict(numpy.array((expression1, expression2, platform)).reshape(1, -1))[0]
        real_x = self.model.predict(numpy.concatenate(condition).reshape(1, -1))[0]

        # print(f"{self.model_expression}\t{real_x}\t{real_x == decide}\t{scene_info['ball'][1]}")
        if real_x != decide:
            b = self.__get_existing_bricks()
            print(
                f"{scene_info['frame']:>4}: {scene_info['ball'][1]}\t{len(b)}\t{real_x} - {decide} = {real_x - decide}", 
                f"\t\t{tuple(b)}", 
                sep="\n"
            )

        command = self.__get_command(scene_info["ball"][1], platform, real_x)

        end = time.time()
        # print(f"{self.__scene_info['frame']}\t{end - start:<10.10f}")

        return self.translate[command]

    def reset(self):
        """
        Reset the status
        """
        self.ball_served = False
        self.__pre_position = None
        self.__pre_velocity = None
        if self.__scene_info["status"] == "GAME_PASS":
            sys.exit(0)
        self.__scene_info = None
    
    pre_position = property(lambda self: self.__pre_position)
    @pre_position.setter
    def pre_position(self, data):
        self.__pre_position = numpy.array(data)

    pre_velocity = property(lambda self: self.__pre_velocity)
    @pre_velocity.setter
    def pre_velocity(self, data):
        self.__pre_velocity = numpy.array(data)
    
    def __start_game(self):
        self.pre_position = self.__get_center_of_ball()
        self.pre_position += (7, 7)  # for calculate first velocity
        self.pre_velocity = (-7, -7)

    def __transform_data(self):
        """ transform scene_info to condition [(position), (velocity), (platform_2end), [bricks]] """

        ball_position = self.__get_center_of_ball()
        ball_velocity = ball_position - self.pre_position
        # print(ball_velocity, end="\t")

        if numpy.linalg.norm(ball_velocity) < numpy.linalg.norm(self.pre_velocity) and ball_position[1] < self.fixed_velocity_offset:
            # if hit something, ball_velocity will not be truly velocity
            inverse_index = numpy.argmax(numpy.abs(ball_velocity - self.pre_velocity))
            ball_velocity[inverse_index] = -self.pre_velocity[inverse_index]
        
        m = ball_velocity[1] / ball_velocity[0]
        expression = numpy.array((m * 10, -1, numpy.dot(ball_position, (-m, 1))))

        condition = [
            expression[::2],
            # [self.__scene_info["platform"][0]],
            self.__mend_bricks(self.__get_existing_bricks())
        ]
        self.model_expression = expression
        
        # store data
        self.pre_position = ball_position
        self.pre_velocity = ball_velocity
        return condition, self.__scene_info["platform"][0]
    
    def __get_existing_bricks(self) -> numpy.ndarray:
        bricks = list()
        for brick in self.__scene_info["bricks"] + self.__scene_info["hard_bricks"]:
            bricks.append(numpy.array(brick))
        return numpy.array(bricks)

    def __get_center_of_ball(self) -> numpy.ndarray:
        return numpy.array(self.__scene_info["ball"], dtype=numpy.float32) + self.ball_size / 2

    def __add_empty_brick(self, add_from: list, brick: numpy.ndarray) -> None:
        add_from.append(brick)

    def __mend_bricks(self, bricks: list, length = 100) -> numpy.ndarray:
        new_bricks = list()

        if len(bricks) <= length:
            for brick in bricks:
                new_bricks.append(numpy.array(brick))
            
            brick = numpy.array((200, 500))
            for _ in range(len(new_bricks), length):
                self.__add_empty_brick(new_bricks, brick)
        
        return numpy.array(new_bricks).ravel()

    def __get_command(self, y, platform, intersection):
        platform_2end = numpy.array((platform, platform + self.platform_width))
        # return command
        if platform_2end[1] - self.platform_edge < intersection:
            return 1  #command = 1
        elif platform_2end[0] + self.platform_edge > intersection:
            return -1  #command = -1
        
        if y > self.cut_offset:
            delta = numpy.abs(platform_2end - intersection)
            return (1, -1)[numpy.argmax(delta)]
        return 0  #command = 0



    platform_expression = numpy.array((0, 1, -400))
    world_width = 200
    brick_size = numpy.array((25, 10))
    side_normal = (  # up, right, down, left
        numpy.array(( 0, -1)), 
        numpy.array(( 1,  0)), 
        numpy.array(( 0,  1)), 
        numpy.array((-1,  0))
    )

    def __transform_data_origin(self):
        """ transform scene_info to condition [(position), (velocity), (platform_2end), [bricks]] """

        ball_position = self.__get_center_of_ball()
        ball_velocity = ball_position - self.pre_position
        # print(ball_velocity, end="\t")

        # print(ball_position, ball_velocity, self.pre_velocity, sep="\t", end="\t")
        if numpy.linalg.norm(ball_velocity) < numpy.linalg.norm(self.pre_velocity) and ball_position[1] < self.fixed_velocity_offset:
            # if hit something, ball_velocity will not be truly velocity
            # print("trans", end="\t")
            inverse_index = numpy.argmax(numpy.abs(ball_velocity - self.pre_velocity))
            ball_velocity[inverse_index] = -self.pre_velocity[inverse_index]
        
        condition = [
            ball_position, 
            ball_velocity, 
            numpy.array((
                self.__scene_info["platform"][0], 
                self.__scene_info["platform"][0] + self.platform_width
            )), 
            self.__get_existing_bricks()
        ]
        return condition

    def __decision(self, condition):
        """ return command """
        position, velocity, platform_2end, bricks = condition
        velocity = copy.deepcopy(velocity)

        m = velocity[1] / velocity[0]
        expression = numpy.array((m, -1, numpy.dot(position, (-m, 1))))
        self.math_expression = expression

        # check whether hit the brick and then find the nearest brick
        ball_brick_distance = -1
        for pos in bricks:
            pos = numpy.array(pos)
            ball_brick = pos - position  # vector ball -> brick
            if numpy.dot(ball_brick, velocity) >= 0 and self.__in_brick_TF(pos, expression):  # hit
                # find the nearest brick
                if ball_brick_distance == -1:
                    self.__brick = pos
                    ball_brick_distance = numpy.linalg.norm(ball_brick)
                    continue
                
                new_brick_ball_distance = numpy.linalg.norm(ball_brick)
                if ball_brick_distance > new_brick_ball_distance:  # nearer
                    self.__brick = pos
                    ball_brick_distance = new_brick_ball_distance

        # calculate the reflection
        if ball_brick_distance != -1:
            position, normal = self.__brick_intersection(self.__brick, expression, position)
            for i in range(2):  # calculate the reflect velocity
                if normal[i] != 0:
                    velocity[i] *= -1
                    break
            # calcualte reflact expression
            m = velocity[1] / velocity[0]
            expression = numpy.array((m, -1, numpy.dot(position, (-m, 1))))
        
        # find the intersection of the platform
        real_x = self.__platform_intersection(expression)
        
        # reset
        self.__brick = None

        return real_x
        # # return command
        # if platform_2end[1] - self.platform_edge < real_x:
        #     return 1  #command = 1
        # elif platform_2end[0] + self.platform_edge > real_x:
        #     return -1  #command = -1
        
        # if position[1] > self.cut_offset:
        #     delta = numpy.abs(platform_2end - real_x)
        #     return (1, -1)[numpy.argmax(delta)]
        # return 0  #command = 0

    def __platform_intersection(self, expression: numpy.ndarray) -> float:
        platform_intersection = self.__solve_linear(expression, self.platform_expression)[0]
        times = platform_intersection // self.world_width + 1
        error_distance = (times + 1 if platform_intersection < 0 else 0) * self.ball_size

        distance = abs(platform_intersection - self.world_width * times)
        return int((self.world_width - distance - error_distance) if times % 2 else (distance + error_distance))

    def __solve_linear(self, exp1: numpy.ndarray, exp2: numpy.ndarray) -> numpy.ndarray:
        coeff = numpy.array(numpy.array((exp1[:-1], exp2[:-1])))
        const = -numpy.array((exp1[-1], exp2[-1]))
        return numpy.linalg.solve(coeff, const)

    def __in_brick_TF(self, brick_pos: numpy.ndarray, expression: numpy.ndarray) -> bool:
        """ check whether the expression across the brick """
        up_left = numpy.concatenate((brick_pos, [1]))
        down_right = numpy.concatenate((brick_pos + self.brick_size, [1]))
        up_right = numpy.concatenate((brick_pos + numpy.array((self.brick_size[0], 0)), [1]))
        down_left = numpy.concatenate((brick_pos + numpy.array((0, self.brick_size[1])), [1]))

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

    def __brick_intersection(self, brick_pos: numpy.ndarray, expression: numpy.ndarray, position: numpy.ndarray) -> tuple[numpy.ndarray, numpy.ndarray]:
        """ return intersection point and normal vector """
        brick = numpy.array((brick_pos, brick_pos + self.brick_size))
        side = (  # up, right, down, left
            numpy.array((0, 1, -(brick[0][1]))),   # y = brick_pos[1]
            numpy.array((1, 0, -(brick[1][0]))),   # x = brick_pos[0] + self.brick_size[0]
            numpy.array((0, 1, -(brick[1][1]))),   # y = brick_pos[1] + self.brick_size[1]
            numpy.array((1, 0, -(brick[0][0])))    # x = brick_pos[0]
        )

        distance = float("inf")
        index = -1
        intersection_point = None
        for i in range(4):  # find which side intersect
            point = self.__solve_linear(expression, side[i])
            if (brick[0] <= point).all() and (point <= brick[1]).all():  # whether the intersection point is on the side
                if new_distance := numpy.linalg.norm(point - position) < distance:  # find the nearest
                    distance = new_distance
                    index = i
                    intersection_point = point
        if index == -1:
            raise Exception("does not intersect")
        
        return (intersection_point, self.side_normal[index])

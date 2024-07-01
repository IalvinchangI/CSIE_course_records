"""
The template of the main script of the machine learning process
"""

import numpy
import pprint
import copy
import time
import random

import ml.collect as collect


class MLPlay:
    bricks_max_length = 100
    translate = {
        -1 : "MOVE_LEFT", 
         0 : "NONE", 
         1 : "MOVE_RIGHT"
    }
    platform_width = 40
    platform_expression = numpy.array((0, 1, -400))
    platform_y = 400

    world_width = 200

    brick_size = numpy.array((25, 10))
    ball_size = 5

    platform_edge = 3

    side_normal = (  # up, right, down, left
        numpy.array(( 0, -1)), 
        numpy.array(( 1,  0)), 
        numpy.array(( 0,  1)), 
        numpy.array((-1,  0))
    )

    def __init__(self,ai_name, *args, **kwargs):
        """
        Constructor
        """
        self.name = ai_name
        self.__scene_info = None
        self.__pre_position = None  # previous tick of ball's position (ndarray)
        self.__pre_velocity = None  # previous tick of ball's velocity (ndarray)
        self.__brick = None
        self.__time_start = time.time()

        self.__pre_hit_platform = 0  # the frame last time the bal hit the platform
        self.__collect = list()  # for collecting data
        self.__store_file = collect.ML_Data(r".\data_linear", r"ml_data.dat", "w")
        print(ai_name)

    pre_position = property(lambda self: self.__pre_position)
    @pre_position.setter
    def pre_position(self, data):
        self.__pre_position = numpy.array(data)

    pre_velocity = property(lambda self: self.__pre_velocity)
    @pre_velocity.setter
    def pre_velocity(self, data):
        self.__pre_velocity = numpy.array(data)
    
    def update(self, scene_info, *args, **kwargs):
        """
        Generate the command according to the received `scene_info`.
        """

        # start = time.time()
        self.__scene_info = scene_info
        # self.__print_time()

        """ preprocess """
        # Make the caller to invoke `reset()` for the next round.
        if (self.__scene_info["status"] == "GAME_OVER" or self.__scene_info["status"] == "GAME_PASS"):
            return "RESET"
        
        # play ball
        if not scene_info["ball_served"]:
            self.__start_game()
            return "SERVE_TO_LEFT"
        
        

        """ play """
        condition = self.__transform_data()

        command = self.__dicision(condition)

        self.__collect_data(condition, command)

        # end = time.time()

        # print(f"\t\t{end - start:<10.10f}")

        return self.translate[command]

    def reset(self):
        """
        Reset the status
        """

        if self.__scene_info["status"] == "GAME_OVER":
            self.__store_file.add_data(self.__clean_data(self.__collect))
        else:
            self.__store_file.add_data(self.__collect)

        self.ball_served = False
        self.__pre_position = None
        self.__pre_velocity = None
        self.__scene_info = None
        self.__collect = list()
        self.__store_file.close()
    
    def __clean_data(self, data: list) -> list:
        final_data = list()
        for frame, thing in enumerate(data):
            if frame < self.__pre_hit_platform:
                final_data.append(thing)
            else:
                break
        return final_data

    def __print_time(self):
        end = time.time()
        print(f"time:\t{self.__scene_info['frame']:<5d} : {end - self.__time_start:10.10f}")
        self.__time_start = end


    def __start_game(self):
        self.pre_position = self.__get_center_of_ball()
        self.pre_position += (7, 7)  # for calculate first velocity
        self.pre_velocity = (-7, -7)
        self.__brick = None

    def __dicision(self, condition):
        """ return command """
        position, velocity, platform_2end, bricks = condition
        velocity = copy.deepcopy(velocity)

        m = velocity[1] / velocity[0]
        expression = numpy.array((m, -1, numpy.dot(position, (-m, 1))))
        
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
        platform_intersection = self.__solve_linear(expression, self.platform_expression)[0]
        times = platform_intersection // self.world_width + 1
        distance = abs(platform_intersection - self.world_width * times)
        real_x = (self.world_width - distance) if times % 2 else distance
        
        # reset
        self.__brick = None

        # return command
        rand = 0#random.randint(-2, 10)
        if platform_2end[1] - self.platform_edge - rand < real_x:
            return 1  #command = 1
        elif platform_2end[0] + self.platform_edge + rand > real_x:
            return -1  #command = -1
        return 0  #command = 0

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
        
    
    def __transform_data(self):
        """ transform scene_info to condition [(position), (velocity), (platform_2end), [bricks]] """

        ball_position = self.__get_center_of_ball()
        ball_velocity = ball_position - self.pre_position

        if numpy.linalg.norm(ball_velocity) < numpy.linalg.norm(self.pre_velocity):
            # if hit something, ball_velocity will not be truly velocity
            inverse_index = numpy.argmax(numpy.abs(ball_velocity - self.pre_velocity))
            ball_velocity[inverse_index] = -self.pre_velocity[inverse_index]

            if (self.pre_position + self.pre_velocity)[1] >= self.platform_y:  # hit platform?
                self.__pre_hit_platform = self.__scene_info["frame"]
        
        condition = [
            ball_position, 
            ball_velocity, 
            numpy.array((
                self.__scene_info["platform"][0], 
                self.__scene_info["platform"][0] + self.platform_width
            )), 
            self.__get_existing_bricks()
        ]
        
        # store data
        self.pre_position = ball_position
        self.pre_velocity = ball_velocity
        return condition
    
    def __get_existing_bricks(self) -> numpy.ndarray:
        bricks = list()
        for brick in self.__scene_info["bricks"] + self.__scene_info["hard_bricks"]:
            bricks.append(numpy.array(brick))
        return numpy.array(bricks)

    def __get_center_of_ball(self) -> numpy.ndarray:
        return numpy.array(self.__scene_info["ball"], dtype=numpy.float32) + self.ball_size / 2

    def __collect_data(self, condition, command):
        """ collect data """
        self.__collect.append({"condition": copy.deepcopy(condition), "action": command})
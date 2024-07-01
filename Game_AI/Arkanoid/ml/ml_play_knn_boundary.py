"""
The template of the main script of the machine learning process
"""

import numpy
import time
import sys

import ml.collect as collect


class MLPlay:
    translate = {
        -1 : "MOVE_LEFT", 
         0 : "NONE", 
         1 : "MOVE_RIGHT"
    }
    platform_width = 40
    platform_expression = numpy.array((0, 1, -400))
    platform_y = 400

    ball_size = 5
    brick_size = numpy.array((25, 10))
    world_width = 200

    side_normal = (  # up, right, down, left
        numpy.array(( 0, -1)), 
        numpy.array(( 1,  0)), 
        numpy.array(( 0,  1)), 
        numpy.array((-1,  0))
    )

    fixed_velocity_offset = platform_y - 10

    def __init__(self,ai_name, *args, **kwargs):
        """
        Constructor
        """
        self.__scene_info = None
        self.__pre_position = None  # previous tick of ball's position (ndarray)
        self.__pre_velocity = None  # previous tick of ball's velocity (ndarray)
        self.__brick = None

        self.model = collect.ML_Data(r".\model", r"ml_fit_boundary_linear2_2.model").read()
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
        condition = self.__decision(self.__transform_data())
        command = self.model.predict(numpy.concatenate(condition).reshape(1, -1))[0]

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
        # if self.__scene_info["status"] == "GAME_PASS":
        #     sys.exit(0)
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
        self.__brick = None

    def __transform_data(self):
        """ transform scene_info to condition [(position), (velocity), (platform_2end), [bricks]] """

        ball_position = self.__get_center_of_ball()
        ball_velocity = ball_position - self.pre_position

        if numpy.linalg.norm(ball_velocity) < numpy.linalg.norm(self.pre_velocity) and ball_position[1] < self.fixed_velocity_offset:
            # if hit something, ball_velocity will not be truly velocity
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

    def __decision(self, condition):
        """ return command """
        position, velocity, platform_2end, bricks = condition

        m = velocity[1] / velocity[0]
        expression = numpy.array((m, -1, numpy.dot(position, (-m, 1))))

        # # check whether hit the brick and then find the nearest brick
        # ball_brick_distance = -1
        # for pos in bricks:
        #     pos = numpy.array(pos)
        #     ball_brick = pos - position  # vector ball -> brick
        #     if numpy.dot(ball_brick, velocity) >= 0 and self.__in_brick_TF(pos, expression):  # hit
        #         # find the nearest brick
        #         if ball_brick_distance == -1:
        #             self.__brick = pos
        #             ball_brick_distance = numpy.linalg.norm(ball_brick)
        #             continue
                
        #         new_brick_ball_distance = numpy.linalg.norm(ball_brick)
        #         if ball_brick_distance > new_brick_ball_distance:  # nearer
        #             self.__brick = pos
        #             ball_brick_distance = new_brick_ball_distance

        # # calculate the reflection
        # if ball_brick_distance != -1:
        #     position, normal = self.__brick_intersection(self.__brick, expression, position)
        #     for i in range(2):  # calculate the reflect velocity
        #         if normal[i] != 0:
        #             velocity[i] *= -1
        #             break
        #     # calcualte reflact expression
        #     m = velocity[1] / velocity[0]
        #     expression = numpy.array((m, -1, numpy.dot(position, (-m, 1))))
        
        # self.__brick = None
        # return (expression[::2], [platform_2end[0]])
        return ([position[1]], expression[::2], [platform_2end[0]])

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
        
    
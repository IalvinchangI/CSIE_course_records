# train KNN

from sklearn.model_selection import train_test_split  # data set
from sklearn.neighbors import KNeighborsClassifier  # model
# from sklearn.metrics import accuracy_score  # test

import os
import numpy
import pprint
import matplotlib.pyplot as plt
import copy
import collect

# '''
world_width = 200
platform_expression = numpy.array((0, 1, -400))
brick_size = numpy.array((25, 10))

platform_edge = 3
platform_width = 40

ball_size = 5

side_normal = (  # up, right, down, left
    numpy.array(( 0, -1)), 
    numpy.array(( 1,  0)), 
    numpy.array(( 0,  1)), 
    numpy.array((-1,  0))
)


def solve_linear(exp1: numpy.ndarray, exp2: numpy.ndarray) -> numpy.ndarray:
    coeff = numpy.array(numpy.array((exp1[:-1], exp2[:-1])))
    const = -numpy.array((exp1[-1], exp2[-1]))
    return numpy.linalg.solve(coeff, const)

def in_brick_TF(brick_pos: numpy.ndarray, expression: numpy.ndarray) -> bool:
    """ check whether the expression across the brick """
    up_left = numpy.concatenate((brick_pos, [1]))
    down_right = numpy.concatenate((brick_pos + brick_size, [1]))
    up_right = numpy.concatenate((brick_pos + numpy.array((brick_size[0], 0)), [1]))
    down_left = numpy.concatenate((brick_pos + numpy.array((0, brick_size[1])), [1]))

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

def brick_intersection(brick_pos: numpy.ndarray, expression: numpy.ndarray, position: numpy.ndarray) -> tuple[numpy.ndarray, numpy.ndarray]:
    """ return intersection point and normal vector """
    brick = numpy.array((brick_pos, brick_pos + brick_size))
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
        point = solve_linear(expression, side[i])
        if (brick[0] <= point).all() and (point <= brick[1]).all():  # whether the intersection point is on the side
            if new_distance := numpy.linalg.norm(point - position) < distance:  # find the nearest
                distance = new_distance
                index = i
                intersection_point = point
    if index == -1:
        raise Exception("does not intersect")
    
    return (intersection_point, side_normal[index])

def decision(condition):
    """ return command """
    position, velocity, platform_2end, bricks = condition
    velocity = copy.deepcopy(velocity)

    m = velocity[1] / velocity[0]
    expression = numpy.array((m, -1, numpy.dot(position, (-m, 1))))

    # """
    # check whether hit the brick and then find the nearest brick
    ball_brick_distance = -1
    for pos in bricks:
        pos = numpy.array(pos)
        ball_brick = pos - position  # vector ball -> brick
        if numpy.dot(ball_brick, velocity) >= 0 and in_brick_TF(pos, expression):  # hit
            # find the nearest brick
            if ball_brick_distance == -1:
                brick = pos
                ball_brick_distance = numpy.linalg.norm(ball_brick)
                continue
            
            new_brick_ball_distance = numpy.linalg.norm(ball_brick)
            if ball_brick_distance > new_brick_ball_distance:  # nearer
                brick = pos
                ball_brick_distance = new_brick_ball_distance

    # calculate the reflection
    if ball_brick_distance != -1:
        position, normal = brick_intersection(brick, expression, position)
        for i in range(2):  # calculate the reflect velocity
            if normal[i] != 0:
                velocity[i] *= -1
                break
        # calcualte reflact expression
        m = velocity[1] / velocity[0]
        expression = numpy.array((m, -1, numpy.dot(position, (-m, 1))))
    
    # """
    """
    return (expression)
    """
    platform_intersection = solve_linear(expression, platform_expression)[0]
    times = platform_intersection // world_width + 1
    error_distance = (times + 1 if platform_intersection < 0 else 0) * ball_size

    distance = abs(platform_intersection - world_width * times)
    return int((world_width - distance - error_distance) if times % 2 else (distance + error_distance))
    # """

def transform_more(condition, pre_bricks: set):
    position, velocity, platform_2end, bricks = condition
    new_bricks = set()
    
    m = velocity[1] / velocity[0]
    expression = numpy.array((m * 10, -1, numpy.dot(position, (-m, 1))))

    new_condition = (
        # [position[1]],
        position,
        # [expression[2]], 
        # expression[::2], 
        velocity, 
        # [platform_2end[0]], 
        # mend_bricks(bricks)
    )
    return new_condition

# '''


def add_empty_brick(add_from: list, brick: numpy.ndarray) -> None:
    add_from.append(brick)

def mend_bricks(bricks: list, length = 100) -> numpy.ndarray:
    new_bricks = list()

    if len(bricks) <= length:
        for brick in bricks:
            new_bricks.append(numpy.array(brick))
        
        brick = numpy.array((200, 500))
        for _ in range(len(new_bricks), length):
            add_empty_brick(new_bricks, brick)
    
    return numpy.array(new_bricks).ravel()

velocity_pool = {(7, 7), (10, 7)}

total = 0
same = 0
def get_data(path, store):
    global total, same
    data_pool = set()
    for file in os.listdir(path):
        store_file = collect.ML_Data(path, file)
        bricks_set = set()
        for dict_data in store_file.read():
            if tuple(numpy.abs(dict_data["condition"][1])) not in velocity_pool:  # clean velocity
                continue
            total += 1
            condition = numpy.concatenate(transform_more(dict_data["condition"], bricks_set))
            # """
            if tuple(condition) in data_pool:
                same += 1
                continue
            data_pool.add(tuple(condition))
            # """
            store["condition"].append(condition)
            store["action"].append(decision(dict_data["condition"]))
            # store["condition"].append(numpy.concatenate(dict_data["condition"][:3]))
            # store["action"].append(dict_data["action"])
            
        store_file.close()


# """
# path = r".\data"
path = r".\data_2"
data = {"condition": list(), "action": list()}
get_data(path, data)
print(same, total)

store = collect.ML_Data(r".\data_pre_cal", "train_Ox_reduce.precal", "w")
store.add_data(data)
store.close()
"""
store = collect.ML_Data(r".\data_pre_cal", "train_Ox.precal")
data = store.read()
store.close()
# """


# plt.scatter(tuple(map(lambda x: x[0], data["condition"])), tuple(map(lambda x: x[1], data["condition"])))
# plt.show()




# """
# split data
x_train, x_test, y_train, y_test = train_test_split(
    data["condition"], data["action"], test_size=0.1
)


# model training
model = KNeighborsClassifier(n_neighbors=1)
model.fit(x_train, y_train)
# model.fit(data["condition"], data["action"])





# model testing
predictions = model.predict(x_test)

correct = 0
incorrect = 0
total = len(y_test)
for actual, predict in zip(y_test, predictions):
    if actual == predict:
        correct += 1
    else:
        incorrect += 1

print(f"{correct = }")
print(f"{incorrect = }")
print(f"accuracy = {100 * correct / total}%")


# store model
store = collect.ML_Data(r".\model", r"ml_fit_Ox.model", "w")
store.add_data(model)
store.close()
# """
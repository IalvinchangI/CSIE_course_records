


from sklearn.model_selection import train_test_split  # data set
from sklearn.neighbors import KNeighborsClassifier  # model
# from sklearn.metrics import accuracy_score  # test

import os
import numpy
import pprint
import matplotlib.pyplot as plt
import collect



platform_edge = 3
platform_width = 40
# def decision(x, platform):
#     # return command
#     if platform + platform_width - platform_edge < x:
#         return 1  #command = 1
#     elif platform + platform_edge > x:
#         return -1  #command = -1
#     return 0  #command = 0

def decision(ball_x, platform_center):
    d = ball_x - platform_center
    if d < 0:
        return -1
    else:
        return 1
    

x = numpy.arange(201)
platform = numpy.arange(20, 181, 5)
mesh = numpy.meshgrid(x, platform)
data = numpy.stack((mesh[0].ravel(), mesh[1].ravel()), axis=1)
action = tuple(map(lambda i: decision(i[0], i[1]), data))

"""
x1 = [i for i in data if dicision(i[0], i[1]) == 1]
x0 = [i for i in data if dicision(i[0], i[1]) == 0]
x_1 = [i for i in data if dicision(i[0], i[1]) == -1]

plt.scatter(*tuple(numpy.array(x1).T), label = "right")
plt.scatter(*tuple(numpy.array(x0).T), label = "none")
plt.scatter(*tuple(numpy.array(x_1).T), label = "left")
plt.legend()
plt.xlabel("x_real")
plt.ylabel("platform")
plt.show()
"""




# split data
x_train, x_test, y_train, y_test = train_test_split(
    data, action, test_size=0.9
)


# model training
model = KNeighborsClassifier(n_neighbors=1)
model.fit(x_train, y_train)


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

# # store model
store = collect.ML_Data(r".\model", r"ml_fit_platform_B.model", "w")
store.add_data(model)
store.close()
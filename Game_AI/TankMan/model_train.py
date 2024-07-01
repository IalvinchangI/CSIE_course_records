import numpy

from ml.train import Trainer
from ml.game_object import Tank
from ml.game_object import PlayGround2 as PlayGround


"""
MODEL_NAME = "dodger"
"""
MODEL_NAME = "shooter"
# """

playGround = PlayGround(
    Tank(numpy.array((int(PlayGround.WIDTH / 2), int(PlayGround.HEIGHT / 2)))), 
    Trainer(r".\model2", MODEL_NAME)
)

Trainer.EPOCH = 10
Trainer.ROUND = 30

playGround.run(Trainer.EPOCH * Trainer.ROUND, 3000)
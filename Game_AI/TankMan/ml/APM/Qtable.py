# model template

import numpy
import random

class Qtable_model():
    def __init__(self, shape: tuple, alpha: float = 0.1, gamma: float = 0.9):
        """
        造 Qtable

        shape : state + action
        """
        # table
        self.__state_shape: tuple = shape[:-1]
        self.__action_count: int = shape[-1]
        self.__Qtable: numpy.ndarray = numpy.zeros(shape, dtype=float)

        # reward and learn
        self.__eGreddy: float = 1  # 探索率
        self.__alpha: float = alpha  # 學習率
        self.__gamma: float = gamma  # 未來的影響率
    
    @property
    def eGreddy(self) -> float:
        return self.__eGreddy
    @eGreddy.setter
    def eGreddy(self, value):
        self.__eGreddy = value

    def __feature_shape_check(self, shape: tuple):
        if len(shape) != len(self.__state_shape):
            raise ValueError(f"out of range: expect {self.__state_shape} but get {shape}")
        # TODO: 檢查是不是 out of range (index的部分)

    def _get_whole_Qtable(self) -> numpy.ndarray:
        return self.__Qtable

    def Qtable(self, *state: int) -> numpy.ndarray:  # copy
        self.__feature_shape_check(state)
        return self.__Qtable[state][:]

    def _Qtable(self, *state: int) -> numpy.ndarray:  # do not copy
        self.__feature_shape_check(state)
        return self.__Qtable[state]

    def update(self, state: tuple, action: int, reward: float, new_state: tuple):
        """
        會在下一frame被call
        state: 上一frame的
        action: 上一frame的
        reward: 從上一frame和這一frame可推知
        next_max_reward: 從這一frame和上一frame可推知
        """
        self.__feature_shape_check(state)
        
        update_array = self._Qtable(*state)
        current = update_array[action]
        update_array[action] = (1 - self.__alpha) * current + self.__alpha * (reward + self.__gamma * numpy.max(self._Qtable(*new_state)))
    
    def get_action(self, *state: int) -> int:  # for training
        if random.random() < self.__eGreddy:  # 比 eGreddy 小 => random
            return random.randint(0, self.__action_count - 1)

        # 比 eGreddy 大 => Qtable
        return self.act(*state)
    
    def act(self, *state: int) -> int:  # just get the best action
        all_action = self._Qtable(*state)
        actions = numpy.where(all_action == numpy.max(all_action))[0]
        return random.choice(actions)

# 控制訓練時 eGreddy 的變化
# 每 EPOCH 就儲存模型

from ml.APM.collect import ML_Data
from ml.APM.robot import Dodger
from ml.APM.transformer import DataTransformer

EXTENSION = ".pickle"

from collections import deque
import numpy


class Trainer():
    """
    屬性:
    1. 紀錄第幾 回合
    2. 記錄第幾 EPOCH

    每回合可以存: 
    1. 上一frame、這一frame的 state
    2. 上一frame的 action
    3. 上一frame的 reward
    4. 上一frame的 score
    5. total_reward

    常數:
    1. 幾回合儲存一次 : (EPOCH)
    2. 總共執行幾個 EPOCH : (ROUND)
    3. 幾個 EPOCH 變一次 eGreddy : (CHANGE_EGREDDY)
    4. action: int -> str
    5. 給動作的評分
    
    數學:
    1. 調整 eGreddy 的函數

    訓練方法:
    1. store_state
    2. update_Qtable
    3. get_action

    結束方法:
    1. clear
    2. decide
    """

    EPOCH = 1  # 幾回合儲存一次
    ROUND = 10  # 總共執行幾個 EPOCH
    CHANGE_EGREDDY = 1  # 幾個 EPOCH 變一次 eGreddy

    DIRECTION_REWARD: tuple = (0.2, -0.5, -1.0, -1.5)
    def __eGreddy_change_value(self, now: float) -> float:
        return now - self.CHANGE_EGREDDY / self.ROUND


    def __init__(self, model_path: str, model_name: str) -> None:
        self.__epoch = 1
        self.__round = 1

        # train
        self.__States: deque = deque(maxlen=2)
        # self.__Actions: deque = deque(maxlen=3)
        self.__Action: int = None
        # self.__Rewards: deque = deque(maxlen=2)
        self.__Reward: float = None
        self.__Score: int = None
        self.__total_reward: int = 0

        # load model
        with ML_Data(model_path, model_name + EXTENSION) as file:
            self.__model_description: dict = file.read()
            # print(self.__model_description)
            self.model = file.read()
        self.__model_path: str = model_path
        self.__model_name: str = model_name.rstrip("0123456789")  # 有把後面的數字切掉

        # data transform
        # if game_level <= 6:
        #     self.transformer = DataTransformer(400, 400)
        # elif game_level == 7:
        #     self.transformer = DataTransformer(500, 500)
        # elif game_level == 8:
        #     self.transformer = DataTransformer(650, 600)

    ############################ history store ############################

    @property
    def State(self) -> tuple:
        """ 上一frame的([0]) """
        if len(self.__States) == 0:
            return None
        return self.__States[0]
    def get_current_state(self) -> tuple:
        """ 這一frame的([-1]) """
        if len(self.__States) == 0:
            return None
        return self.__States[-1]
    @State.setter
    def State(self, value: tuple):
        self.__States.append(value)
    
    @property
    def Action(self) -> int:
        return self.__Action
    @Action.setter
    def Action(self, value: int):
        self.__Action = value
    
    @property
    def Reward(self) -> float:
        return self.__Reward
    @Reward.setter
    def Reward(self, value: float):
        self.__Reward = value
    
    @property
    def Score(self) -> float:
        return self.__Score
    @Score.setter
    def Score(self, value: float):
        self.__Score = value
    
    '''
    @property
    def Action(self) -> int:
        if len(self.__Actions) < 2:
            return None
        return self.__Actions.popleft()
    @Action.setter
    def Action(self, value: int):
        self.__Actions.append(value)
    
    @property
    def Reward(self) -> float:
        if len(self.__Rewards) < 1:
            return None
        return self.__Rewards.popleft()
    def get_current_reward(self) -> float:
        return self.__Rewards[-1]
    @Reward.setter
    def Reward(self, value: float):
        self.__Rewards.append(value)
    '''
    
    def clear(self) -> None:
        self.__States.clear()
        self.__Action = None
        # self.__Actions.clear()
        self.__Reward = None
        # self.__Rewards.clear()
        self.__Score = None
        self.__total_reward = 0
        # self.transformer.clear()

    ############################ 為下一回合做準備 ############################

    def __store(self) -> None:
        """ store model to file """
        description = self.__model_description.copy()
        # 調整 description 的 round, total_reward, final_score
        description["round"] = self.__round
        description["total_reward"] = self.__total_reward
        description["final_score"] = self.Score

        with ML_Data(self.__model_path, self.__model_name + EXTENSION, mode="w") as file:
            description["name"] = file.file_name  # 調整 description 的 name
            print(f"name = {description['name']}\tround = {description['round']}\ttotal_reward = {description['total_reward']}\tfinal_score = {description['final_score']}")
            file.add_data(description)
            file.add_data(self.model)

    def __decide_end_train(self) -> None:
        if self.__round == self.ROUND and self.__epoch == self.EPOCH:  # end training and store
            self.__store()
            raise Exception("=== END TRIANING ===")

    def __decide_store_model(self) -> None:
        """ if store, it will change epoch(=0) and round(+1) """
        if self.__epoch % self.EPOCH != 0:  # not store
            return None
        # store
        self.__store()
        
        # reset epoch and update round
        self.__epoch = 0  # 因為等下會 +1
        self.__round += 1

    def __decide_change_eGreddy(self) -> None:
        if (self.__epoch == self.EPOCH) and ((self.__round % self.CHANGE_EGREDDY) == 0):  # 改 eGreddy
            self.model._Q_table.eGreddy = self.__eGreddy_change_value(self.model._Q_table.eGreddy)
            print(f"update eGreddy as {self.model._Q_table.eGreddy} at {self.__round}")

    def __increase(self) -> None:
        """ update epoch """
        self.__epoch += 1
    
    def decide(self) -> None:
        """
        為下一回合做準備:
        1. 決定是否結束訓練
        2. 決定是否儲存模型
        3. 決定是否更換 eGreddy
        4. 更新 EPOCH
        """
        self.__decide_end_train()
        self.__decide_change_eGreddy()
        self.__decide_store_model()
        self.__increase()

    ############################ train ############################

    def reward_function(self, score: int, score_accumulate: int) -> float:
        """
        回傳 reward:
        1. 分數(s)照給
        """
        reward = score + 0.07 * numpy.clip(score_accumulate, -100, 100)
        return reward

    def store_state(self, state: tuple) -> None:
        """ 儲存 state(S(n+1)) """
        self.State = state  # S(n+1)

    def update_Qtable(self, score: int, score_accumulate: int) -> None:
        """
        1. 儲存 reward(R(n)), score
        2. 更新儲存的模型的 Qtable
        """
        # store reward, score
        self.Score = score
        self.Reward = self.reward_function(score, score_accumulate)  # R(n)
        self.__total_reward += self.Reward
        # update and pop
        self.model.update(self.State, self.Action, self.Reward, self.get_current_state())

    def get_action(self) -> int:
        """
        1. 從模型那得到 action
        2. 儲存 action
        """
        # get action
        action = self.model.train_act(*self.get_current_state())
        # print(f"frame = {scene_info['frame']}\tstate = {self.get_current_state()}\taction = {action}")

        # store action
        self.Action = action

        return action

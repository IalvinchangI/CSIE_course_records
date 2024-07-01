# collect and clean data

import os
import pickle


class ML_Data():
    def __init__(self, path: str, name: str, mode: str = "r"):
        self.__store_path = path
        self.__file_path = os.path.join(self.__store_path, name)
        self.__file_name, self.__file_extension = name.split(".")
        self.__mode = mode
        if mode == "r":
            self.__open()
        elif mode == "w":
            self.__new()
        else:
            raise ValueError()

    def __open(self) -> None:
        if os.path.exists(self.__file_path):
            self.__file = open(self.__file_path, mode="rb")
        else:
            self.__new()
            self.__mode = "w"

    def __new(self) -> None:
        file_name = self.__file_name
        i = 1
        while True:
            if os.path.exists(self.__file_path):  # File Exists!!
                self.__file_name = file_name + str(i)
                self.__file_path = os.path.join(self.__store_path, self.__file_name + "." + self.__file_extension)
                i += 1
            else:
                self.__file = open(self.__file_path, mode="wb")
                break
    
    def add_data(self, data) -> None:
        if self.__mode == "w":
            pickle.dump(data, self.__file)
        else:
            raise AttributeError
    
    def read(self):
        if self.__mode == "r":
            return pickle.load(self.__file)
        else:
            raise AttributeError

    def close(self) -> None:
        self.__file.close()
        

if __name__ == "__main__":
    __file = r"ml_data.dat"
    ML_Data(__file, "w")
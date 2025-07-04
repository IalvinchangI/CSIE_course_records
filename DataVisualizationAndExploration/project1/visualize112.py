import matplotlib.pyplot as plt
import seaborn
import pandas
import os

os.chdir(r".\DataVisualizationAndExploration\project1")

INDEX_CILUMN_NAME = "區域別"
COLUMN_NAMES = (INDEX_CILUMN_NAME, "長照需求服務涵蓋率(F=E/A)")
HIGHLIGHT = ["新北市", "臺北市", "桃園市", "臺南市", "新竹市", "基隆市"]
DATA_PATH = "112data.csv"

# data
data = pandas.read_csv(DATA_PATH)
data_F: pandas.DataFrame = data.loc[1:, COLUMN_NAMES].set_index(INDEX_CILUMN_NAME)
data_F["長照需求服務涵蓋率(F=E/A)"] = data_F["長照需求服務涵蓋率(F=E/A)"].apply(lambda x: float(x.replace("%", "")))
data_F.sort_values("長照需求服務涵蓋率(F=E/A)", ascending=False, inplace=True)

# print(data_F)


# plot1
figure = plt.figure(1, figsize=(12, 8))

plt.axvline(100, ymin=0, ymax=1, linestyle="--", color="black", linewidth=1, zorder=0)
plt.text(100, -0.75, "100%", horizontalalignment="center")

seaborn.barplot(
    data_F, x="長照需求服務涵蓋率(F=E/A)", y=INDEX_CILUMN_NAME, 
    color="#4b8cc6", legend=False
)

for i in range(len(data_F)):
    plt.text(
        data_F["長照需求服務涵蓋率(F=E/A)"][i] - 1, i + 0.2, 
        f"{data_F['長照需求服務涵蓋率(F=E/A)'][i]}%", horizontalalignment="right", 
        color="white"
    )

plt.suptitle("各縣市長照2.0需求服務涵蓋率", fontsize=25, fontweight="bold", family="Microsoft JhengHei")
plt.xlabel("長照需求服務涵蓋率", fontsize=18, fontweight="bold", family="Microsoft JhengHei")
plt.ylabel("縣市", fontsize=18, fontweight="bold", family="Microsoft JhengHei")
plt.yticks(range(len(data_F)), data_F.index, family="Microsoft JhengHei")
plt.xticks([])
plt.gca().set_frame_on(False)

plt.savefig("112_F_column")


# plot2
figure = plt.figure(2, figsize=(12, 8))

plt.axvline(100, ymin=0, ymax=1, linestyle="--", color="black", linewidth=1, zorder=0)
plt.text(100, -0.75, "100%", horizontalalignment="center")

seaborn.barplot(
    data_F, x="長照需求服務涵蓋率(F=E/A)", y=INDEX_CILUMN_NAME, hue=INDEX_CILUMN_NAME, 
    palette=dict(zip(data_F.index, map(lambda x: "#f1b457" if x in HIGHLIGHT else "#4b8cc6", data_F.index))), legend=False
)

for i in range(len(data_F)):
    plt.text(
        data_F["長照需求服務涵蓋率(F=E/A)"][i] - 1, i + 0.2, 
        f"{data_F['長照需求服務涵蓋率(F=E/A)'][i]}%", horizontalalignment="right", 
        color="white"
    )

plt.suptitle("各縣市長照2.0需求服務涵蓋率", fontsize=25, fontweight="bold", family="Microsoft JhengHei")
plt.xlabel("長照需求服務涵蓋率", fontsize=18, fontweight="bold", family="Microsoft JhengHei")
plt.ylabel("縣市", fontsize=18, fontweight="bold", family="Microsoft JhengHei")
plt.yticks(range(len(data_F)), data_F.index, family="Microsoft JhengHei")
plt.xticks([])
plt.gca().set_frame_on(False)

plt.savefig("112_F_column_highlight")
plt.show()
#90bde6

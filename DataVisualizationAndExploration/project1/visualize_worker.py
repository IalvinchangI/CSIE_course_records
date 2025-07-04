import matplotlib.pyplot as plt
import pandas
import os

os.chdir(r".\DataVisualizationAndExploration\project1")

DATA_PATH = "worker.csv"

# data
data = pandas.read_csv(DATA_PATH).set_index("長照人力")
data = data.div(data.sum(axis=1), axis=0)

# plot
plt.figure(figsize=(8, 6))
for i in range(4):
    plt.subplot(2, 2, i + 1)
    plt.pie(
        data.iloc[i].T, 
        labels=data.columns, autopct="%1.1f%%", 
        textprops={"family": "Microsoft JhengHei"}, 
        colors=["#4b8cc6", "#f1b457"]
    )
    plt.title(data.index[i], fontsize=12, fontweight="bold", family="Microsoft JhengHei")

plt.savefig("112_worker")
plt.show()

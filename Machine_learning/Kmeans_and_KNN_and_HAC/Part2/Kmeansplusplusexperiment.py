"""
Name: Talal Shafei
ID: 2542371

this file is the same file as Kmeansexperiment.py
the only difference is that we are using KMeansPlusPlus class in the loop
"""

from KMeansPlusPlus import KMeansPlusPlus
import pickle
import numpy as np
import matplotlib.pyplot as plt


dataset1 = pickle.load(open("../data/part2_dataset_1.data", "rb"))
dataset2 = pickle.load(open("../data/part2_dataset_2.data", "rb"))


"""
will try K values from 2 to 10

that means 9 attempts
"""

datasets = {"Dataset 1": dataset1, "Dataset 2": dataset2}

stats_dict = {"Dataset 1": np.zeros((9, 3)), "Dataset 2": np.zeros((9, 3))}
loss_lst = []
lowest_lst = []


for Dname, X in datasets.items():
    print()
    print(f"For {Dname}:")
    print("*"*80)
    for K in range(2, 11):
        # the mean loss of 10 times and confidence interval.
        # we will use it to draw the graph and pick the best K
        loss_lst.clear()
        for _ in range(10):

            # pick the lowest loss
            lowest_lst.clear()
            for _ in range(10):

                model = KMeansPlusPlus(X, K)

                _, _, loss = model.run()

                lowest_lst.append(loss)

            loss_lst.append(np.min(lowest_lst))

        Mean = np.mean(loss_lst)
        Std = np.std(loss_lst)
        lower = Mean - 1.96*Std/np.sqrt(10.0)
        upper = Mean + 1.96*Std/np.sqrt(10.0)

        # storring mean loss
        stats_dict[Dname][K-2][0] = Mean

        # storring confidence interval
        stats_dict[Dname][K-2][1] = lower
        stats_dict[Dname][K-2][2] = upper

        print(f"For K = {K}:")
        print(
            f"Mean Loss equals {Mean:.3f} with 95% Confidence Interval [ {lower:.3f}, {upper:.3f} ]")


def plot_elbow(loss_means, Dname="Dataset"):
    plt.figure(figsize=(10, 8))
    plt.plot(range(2, 11), loss_means, marker="o")
    plt.title(f"Loss vs. K for {Dname}", fontsize="28", pad=20)
    plt.xlabel("K", fontsize="23")
    plt.ylabel("Loss", fontsize="23")
    plt.grid()
    # plt.savefig(f"figures/kmeansplusplus/{Dname}.png")
    plt.show()


for Dname in datasets.keys():
    plot_elbow(stats_dict[Dname][:, 0], Dname)

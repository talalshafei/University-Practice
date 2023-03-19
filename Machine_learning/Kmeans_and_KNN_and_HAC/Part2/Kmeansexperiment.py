"""
Name: Talal Shafei
ID: 2542371

"""

import matplotlib.pyplot as plt
from KMeans import KMeans
import numpy as np
import pickle


dataset1 = pickle.load(open("../data/part2_dataset_1.data", "rb"))
dataset2 = pickle.load(open("../data/part2_dataset_2.data", "rb"))


"""
will try K values from 2 to 10

that mean 9 values
"""

# loaded Datasets
datasets = {"Dataset 1": dataset1, "Dataset 2": dataset2}

# we will stat_dict store the Mean loss on all the iteration (mean of loss_lst) for the 9 values and the confidence interval
stats_dict = {"Dataset 1": np.zeros((9, 3)), "Dataset 2": np.zeros((9, 3))}

# we will store in loss_lst the minimum loss in lowest_lst
loss_lst = []
# we will store in lowest_lst the 10 losses on the same k
lowest_lst = []

# loop to choose the dataset
for Dname, X in datasets.items():
    print()
    print(f"For {Dname}:")
    print("*"*80)
    for K in range(2, 11):
        # claring th loss_lst for the new K
        loss_lst.clear()
        for _ in range(10):
            # claering the lowest_lst for the new 10 iterations
            lowest_lst.clear()
            for _ in range(10):
                # building the model
                model = KMeans(X, K)

                # getting the loss
                _, _, loss = model.run()

                # append it
                lowest_lst.append(loss)

            # append the minimum loss
            loss_lst.append(np.min(lowest_lst))

        # statistics of the losses for K clusters
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

# function to plot loss vs K


def plot_elbow(loss_means, Dname="Dataset"):
    # setting the figure size
    plt.figure(figsize=(10, 8))

    # drawing lines between the given points, with marking the point with colored circle
    plt.plot(range(2, 11), loss_means, marker="o")

    # setting the title and labels
    plt.title(f"Loss vs. K for {Dname}", fontsize="28", pad=20)
    plt.xlabel("K", fontsize="23")
    plt.ylabel("Loss", fontsize="23")
    plt.grid()
    # plt.savefig(f"figures/kmeans/{Dname}.png")
    plt.show()


# drawing from figures
for Dname in datasets.keys():
    plot_elbow(stats_dict[Dname][:, 0], Dname)

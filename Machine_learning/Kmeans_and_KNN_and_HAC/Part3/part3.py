"""
Name: Talal Shafei
ID: 2542371

"""

import pickle
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from sklearn.cluster import AgglomerativeClustering
from sklearn.metrics import silhouette_score, silhouette_samples
from scipy.cluster.hierarchy import dendrogram


dataset = pickle.load(open("../data/part3_dataset.data", "rb"))

"""
4 hyperparameter configurations

linkage: "single", "complete"
Affinity: "euclidean", "cosine"

"""


# plot_dendrogram was formed based on codes from scikit-learn documentation
# https://scikit-learn.org/stable/auto_examples/cluster/plot_agglomerative_dendrogram.html
def plot_dendrogram(clusters, **kwargs):

    # store the count of the points in each cluster
    counts = np.zeros(clusters.children_.shape[0])
    num_clusters = len(clusters.labels_)

    # since sklearn procedure for the children: Values less
    # than num_clusters correspond to leaves of the tree which are the
    # original points. A node i greater than or equal to num_clusters
    # is a non-leaf node (cluster) and has children = children_[i - num_clusters].
    for i, merge in enumerate(clusters.children_):
        # merge is 2D vector of two clusters that will merge
        # (if there values is less than num_clusters they refer to points)
        # the result of the merge can be found in children[i + num_clusters]
        # so to count the number of points in each cluster we will check if it is points inside the
        # merge we add +1 if it cluster we add all its children[i - num_clusters]
        # also sice children is sorted we can iterate easily and check the points before reaching their cluster

        # two iteration loop
        cluster_i_count = 0
        for child in merge:
            if child < num_clusters:
                cluster_i_count += 1
            else:
                # add the count of all the points in the inner cluster to the count of the outer cluster
                cluster_i_count += counts[child - num_clusters]

        counts[i] = cluster_i_count

    # stack the three matrices by columns
    linkage_matrix = np.column_stack(
        [clusters.children_, clusters.distances_, counts])

    # Plot the corresponding dendrogram using Scipy
    dendrogram(linkage_matrix, **kwargs)

    # labeling axes
    plt.ylabel("Distance", fontsize="16")
    plt.xlabel(
        "Number of points in the final clusters (or index of point if no pararenthesis).", fontsize="16")


# creating hyperparameter configurations list
hyperparameters = [(link, dist) for link in ["single", "complete"]
                   for dist in ["euclidean", "cosine"]]


# Plotting Dendrograms
print("Plotting Dendrograms")
print("*"*80)
print()
for link, dist in hyperparameters:
    # build the model
    clusters = AgglomerativeClustering(
        n_clusters=None, distance_threshold=0, linkage=link, affinity=dist)

    # fitting the model on the data
    clusters.fit(dataset)

    # setting size of the figure and the title
    plt.figure(figsize=(10, 8))
    plt.title(f"For {link} linkage and {dist} distance", fontsize="25", pad=20)

    # plot the top three levels of the dendrogram
    plot_dendrogram(clusters, truncate_mode="level", p=3)

    # saving the figure
    # this part will be commented
    # plt.savefig(f"figures/{link}_{dist}_dendrogram.png")
    plt.show()


# Silhouette Analysis

"""
16 hyperparameter configurations

linkage: "single", "complete"
Affinity: "euclidean", "cosine"
K: 2, 3, 4, 5
"""

# plot_silhouette was formed based on codes from scikit-learn documentation
# https://scikit-learn.org/stable/auto_examples/cluster/plot_kmeans_silhouette_analysis.html#sphx-glr-auto-examples-cluster-plot-kmeans-silhouette-analysis-py


def plot_silhoutte(X, labels, sim_fun, num_clusters, sil_avg):

    # we get the the silhoutte value for each point in its cluster
    sil_values = silhouette_samples(X, labels, metric=sim_fun)

    # set the initial value for y_lower as 10 so we can fill between y_lower and y_upper
    # based on the sil_values for each point inside its cluster
    y_lower = 10

    # looping on all the clusters
    for cluster_i in range(num_clusters):

        # fetching the cluster silhouette values by masking
        cluster_i_sil_values = sil_values[labels == cluster_i]
        # sorting so when we fill we start by drawing line for the smallest
        # so we can have the "knife down" shape
        cluster_i_sil_values.sort()

        # cluster size is how many points inside it, we can also use len() here
        cluster_i_size = cluster_i_sil_values.shape[0]

        # setting y_upper to y_lower plus the size becasu we will draw lines between them equal to size
        y_upper = y_lower + cluster_i_size

        # to change the color for each cluster
        color = cm.nipy_spectral(float(cluster_i) / num_clusters)

        # fill between y_ lower and y_ ypper starting from x =0 to x = silhouette value of the point
        plt.fill_betweenx(
            y=np.arange(y_lower, y_upper),
            x1=0,
            x2=cluster_i_sil_values,
            facecolor=color,
            edgecolor=color,
            alpha=0.7
        )
        # setting the location of the index of the cluster to the little left and middle of the drawing
        plt.text(-0.05, y_lower + 0.5*cluster_i_size, str(cluster_i))

        # setting the new y_lower to y_upper + 10
        y_lower = y_upper + 10

    # setting labels of the figure
    plt.xlabel("The silhouette coefficient values", fontsize="16")
    plt.ylabel("Cluster label", fontsize="16")

    # The vertical line for average silhouette score of all the values in all the clusters
    plt.axvline(x=sil_avg, color="red", linestyle="--")

    # Clear the yaxis labels / ticks
    plt.yticks([])
    # setting the limit of x to 1 since silhoette value is bounded [-1, 1]
    plt.xlim(right=1.0)


# list to store the silhoette averages to compare them later
sil_scores = []
# hyper parameter configuration with the K values
hyper = [(link, dist, num_clusters)
         for link, dist in hyperparameters for num_clusters in range(2, 6)]

print("Silhoette Analysis")
print("*"*80)
print()
for link, dist, num_clusters in hyper:
    # building the model
    clusterer = AgglomerativeClustering(
        num_clusters, linkage=link, affinity=dist)

    # fitting the models and returning the labels
    labels = clusterer.fit_predict(dataset)

    # compute the average silhoette value
    sil_avg = silhouette_score(dataset, labels, metric=dist)
    # append it to the list
    sil_scores.append(sil_avg)

    # printing the details of the loop
    print(
        f"For {num_clusters} clusters with {link} linkage and {dist} distance metric")
    print(f"Average Silhouette Score: {sil_avg:.4f}")
    print()

    # plotting the silhoette figures for each hyperparameter configuration
    # setting the size of the figure
    plt.figure(figsize=(10, 8))
    plt.title(
        f"For the {num_clusters} clusters, {link} linkage and {dist} distance", fontsize="25", pad=20)
    plot_silhoutte(dataset, labels, dist, num_clusters, sil_avg)

    # saving the figure
    # this part will be commented
    # plt.savefig(f"figures/{link}_{dist}_{num_clusters}_silhouette.png")
    plt.show()

# getting the index of the max average silhoette value and printing the
max_sil = np.argmax(sil_scores)
print(f"Highest Silhouette value: {sil_scores[max_sil]}")
print("The Best Hyperparameter Configuration Based on Average Silhoutte score:")
print(
    f"is the {max_sil + 1}th configuration, with {hyper[max_sil][2]} Clusters {hyper[max_sil][0]} Linkage and {hyper[max_sil][1]} Distance   ")

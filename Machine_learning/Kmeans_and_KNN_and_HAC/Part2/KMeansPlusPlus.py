"""
Name: Talal Shafei
ID: 2542371

this file is the same file as KMeans.py
the only difference is the initializer function
"""

import numpy as np
from Distance import Distance


class KMeansPlusPlus:
    def __init__(self, dataset, K=2):
        """
        :param dataset: 2D numpy array, the whole dataset to be clustered
        :param K: integer, the number of clusters to form
        """
        self.K = K
        self.dataset = dataset
        # each cluster is represented with an integer index
        # self.clusters stores the data points of each cluster in a dictionary
        self.clusters = {i: [] for i in range(K)}
        # self.cluster_centers stores the cluster mean vectors for each cluster in a dictionary
        self.cluster_centers = {i: None for i in range(K)}

        # identity matrix to use it in Mahalanobis Distance to get Euclidean Distance
        self.id = np.identity(dataset.shape[1])

    def calculateLoss(self):
        """Loss function implementation of Equation 1"""
        loss = 0
        for i, cluster in self.clusters.items():
            m = self.cluster_centers[i]
            for x in cluster:

                # calculating the loss of each point in the cluster and add it to the whole loss
                # Mahalanobis with the idintity matrix will behave like Euclidean distance
                # squaring before adding to the sum
                loss += np.square(Distance.calculateMahalanobisDistance(x, m, self.id))

        return loss

    def centers_initializer(self):
        # Kmeans++ initializer
        # step 1 choose centroid from the examples unifomrly
        uniform_random = np.random.randint(0, self.dataset.shape[0])
        self.cluster_centers[0] = self.dataset[uniform_random]

        # step 2 the probability of the next center will be equals to
        # D(x)^2 / (sum i to n D(xi)^2)
        # D(x) is the distance between x and the closest center to x

        D_x2 = np.inf
        # list to store the squared distance so at the end we can compute the probability
        D_list = []
        for i in range(self.K):

            D_list.clear()
            indexes = np.arange(self.dataset.shape[0])
            for index, x in enumerate(self.dataset):

                # if x is an initailized centroid ignore it and loop again
                # also remove its index from the indexes that we want to compute probability to
                if list(x) in [list(i) for i in self.cluster_centers.values() if i is not None]:
                    indexes = indexes[indexes != index]
                    continue

                # set D(x)^2 to infinity
                D_x2 = np.inf

                for center in self.cluster_centers.values():
                    # stop because we finished the initalized centroids
                    if center is None:
                        break

                    # calculate the Distance between x and the centroid
                    distance = Distance.calculateMahalanobisDistance(
                        x, center, self.id)

                    # squared
                    distance = distance**2
                    # keep the distance between x and the nearest centroid
                    if distance < D_x2:
                        D_x2 = distance

                # append the minumum distance
                D_list.append(D_x2)

            # array of size = number of examples - initialized clusters
            probability = np.array(D_list) / np.sum(D_list)

            # pick a random index based on the probability distribution computed above
            # the less the distance the less chance the index will be picked
            weighted_random = np.random.choice(indexes, p=probability)
            # set the new centroid
            self.cluster_centers[i] = self.dataset[weighted_random]

    def run(self):
        """Kmeans++ algorithm implementation"""

        # initialize with Kmean++ initializer
        self.centers_initializer()

        # then proceed like the rest of the Kmeans algorithm

        old_centers = [None for i in range(self.K)]
        centers = [list(i) for i in self.cluster_centers.values()]

        while np.all(old_centers != centers):

            old_centers = centers.copy()
            # E step
            # clearing the old clusters
            self.clusters.clear()
            self.clusters = {i: [] for i in range(self.K)}
            for x in self.dataset:
                min_distance = np.inf
                label = 0

                for j, center in self.cluster_centers.items():
                    distance = Distance.calculateMahalanobisDistance(
                        x, center, self.id)
                    if distance < min_distance:
                        min_distance = distance
                        label = j

                self.clusters[label].append(x)

            # M step

            for k, cluster in self.clusters.items():
                # only computer the center if there are elements in the cluster
                # if there is none leave the center as it is
                if cluster != []:
                    self.cluster_centers[k] = np.mean(
                        np.array(cluster), axis=0)

            centers = [list(i) for i in self.cluster_centers.values()]

        return self.cluster_centers, self.clusters, self.calculateLoss()

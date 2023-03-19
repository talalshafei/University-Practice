"""
Name: Talal Shafei
ID: 2542371

"""

from Distance import Distance
import numpy as np


class KMeans:
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
            # getting the centroid of the cluster
            m = self.cluster_centers[i]

            for x in cluster:
                # calculating the loss of each point in the cluster and add it to the whole loss
                # Mahalanobis with the idintity matrix will behave like Euclidean distance
                # squaring before adding to the sum
                loss += np.square(Distance.calculateMahalanobisDistance(x, m, self.id))

        return loss

    def run(self):
        """Kmeans algorithm implementation"""

        # first initialize the centers randomly
        for i in range(self.K):
            random_index = np.random.randint(0, self.dataset.shape[0])
            self.cluster_centers[i] = self.dataset[random_index]

        # making it in a list format for the while condition
        old_centers = [None for i in range(self.K)]
        centers = [list(i) for i in self.cluster_centers.values()]

        # if the old centroids are not the same as the new ones that means the
        # Kmeans didn't converge and we need to loop again
        while np.all(old_centers != centers):

            # setting the old centroids
            old_centers = centers.copy()

            # E step
            # clearing the old clusters
            self.clusters.clear()
            self.clusters = {i: [] for i in range(self.K)}
            for x in self.dataset:

                # finding the nearest centroid for each point
                min_distance = np.inf
                label = 0
                for j, center in self.cluster_centers.items():
                    distance = Distance.calculateMahalanobisDistance(
                        x, center, self.id)
                    if distance < min_distance:
                        min_distance = distance
                        label = j

                # assigning the point to the cluster with nearest centroid
                self.clusters[label].append(x)

            # M step
            # computing the centroid of the cluster
            for k, cluster in self.clusters.items():
                # only compute the center if there are elements in the cluster
                # if there is none (cluster is empty) leave the center as it is
                if cluster != []:
                    self.cluster_centers[k] = np.mean(
                        np.array(cluster), axis=0)

            # setting the new clusters for the while
            centers = [list(i) for i in self.cluster_centers.values()]

        return self.cluster_centers, self.clusters, self.calculateLoss()

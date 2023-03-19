"""
Name: Talal Shafei

"""

import numpy as np

# will use mode function to find the most repeated label in an array of size K,
# to help with th majority voting


def mode(arr):
    dic = {}
    for i in arr:
        # counting how many i appeared the array
        if i in dic:
            dic[i] += 1
        else:
            dic[i] = 0

    # storing the value of the most repeated
    most_repeated = max(dic.values())

    # return i if i is corresponds to the most repeated return i
    for i, j in dic.items():
        if j == most_repeated:
            return i


class KNN:
    def __init__(self, dataset, data_label, similarity_function, similarity_function_parameters=None, K=1):
        """
        :param dataset: dataset on which KNN is executed, 2D numpy array
        :param data_label: class labels for each data sample, 1D numpy array
        :param similarity_function: similarity/distance function, Python function
        :param similarity_function_parameters: auxiliary parameter or parameter array for distance metrics
        :param K: how many neighbors to consider, integer
        """
        self.K = K
        self.dataset = dataset
        self.dataset_label = data_label
        self.similarity_function = similarity_function
        self.similarity_function_parameters = similarity_function_parameters

    def predict(self, instance):

        # initialize distances as an array of size equal to the number of examples
        distances = np.zeros(self.dataset.shape[0])

    # initialize predictions to an array of size equal to number of instances
        predictions = np.zeros(instance.shape[0])

    # the outer loop will loop over all instances to predict each one an append it to predictions array
        for i in range(predictions.shape[0]):
            # the inner loop will calculate the distance of the instance and each example in the dataset
            for j in range(distances.shape[0]):

                # if the distance function has extra parameters pass them
                if self.similarity_function_parameters is not None:
                    distances[j] = self.similarity_function(
                        instance[i], self.dataset[j], self.similarity_function_parameters)
                else:
                    distances[j] = self.similarity_function(
                        instance[i], self.dataset[j])

            # indices of the K closest Neighbors
            indices = np.argsort(distances)[:self.K]

            # apply majority voting
            predictions[i] = mode(self.dataset_label[indices])

        # return predictions
        return predictions

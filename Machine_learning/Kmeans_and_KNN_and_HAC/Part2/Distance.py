import numpy as np


class Distance:
    @staticmethod
    def calculateCosineDistance(x, y):
        # the bigger the cosine, the less the distances
        cosine = np.dot(x, y) / (np.linalg.norm(x) * np.linalg.norm(y))
        return np.arccos(cosine)

    @staticmethod
    def calculateMinkowskiDistance(x, y, p=2):
        sum_diff = 0
        for i in range(len(x)):
            sum_diff += abs(x[i] - y[i]) ** p

        return sum_diff ** (1/p)

    @staticmethod
    def calculateMahalanobisDistance(x, y, S_minus_1):
        return np.sqrt((x-y).T @ S_minus_1 @ (x-y))

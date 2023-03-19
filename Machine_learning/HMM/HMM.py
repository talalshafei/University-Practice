# Name: Talal Shafei

import numpy as np


class HMM:

    def __init__(self, A, B, Pi):
        self.A = A
        self.B = B
        self.Pi = Pi

    def forward_log(self, O: list):
        """
        :param O: is the sequence (an array of) discrete (integer) observations, i.e. [0, 2,1 ,3, 4]
        :return: ln P(O|λ) score for the given observation, ln: natural logarithm
        """
        # set the variables we are going to use frequently
        T = len(O)
        A = self.A
        B = self.B
        # set c to vector of length equals to the observation sequence
        c = np.zeros(T)

        # set alpha to vector of the same length of Pi (means the number of states)
        alpha = self.Pi * B[:, O[0]]
        # find c at time 0
        c[0] = 1 / np.sum(alpha)
        # normalize alpha
        alpha = alpha * c[0]

        # loop to traverse the whole list of observation
        for t in range(1, T):
            # update alpha based on the formulas we can obtain the first
            # term as dot product between alpha and A
            # also multiply by the probability of the observation for each state
            # which will result in a vector alpha of length equal to stats
            alpha = np.dot(alpha, A) * B[:, O[t]]
            # find c_t
            c[t] = 1 / np.sum(alpha)
            # normalize alpha
            alpha = alpha * c[t]

            # return ln P(O|λ) which is equal to the negative sum of logs of elements in c
        return -np.sum(np.log(c))

    def viterbi_log(self, O: list):
        """
        :param O: is an array of discrete (integer) observations, i.e. [0, 2,1 ,3, 4]
        :return: the tuple (Q*, ln P(Q*|O,λ)), Q* is the most probable state sequence for the given O
        """

        # set the variables we are going to use frequently
        T = len(O)
        A = self.A
        B = self.B
        N = A.shape[0]

        # Initialization step
        delta = np.empty((T, N))
        delta[0, :] = np.log(self.Pi) + np.log(B[:, O[0]])

        phi = np.empty((T, N), dtype='int')
        for j in range(N):
            tmp = delta[0, :] + np.log(A[:, j])
            phi[0, j] = np.argmax(tmp)

        # Recursive step
        # as in the formulas in the manual
        # where ':' means exploring all i column instead of doing extra loop
        for t in range(1, T):
            for j in range(N):
                tmp = delta[t - 1, :] + np.log(A[:, j])
                delta[t, j] = np.max(tmp) + np.log(B[j, O[t]])
                phi[t, j] = np.argmax(tmp)

        # Path (state sequence) backtracking step
        # initialize Q_star and put the last element as the given formula
        Q_star = np.empty(T, dtype='int')
        Q_star[T - 1] = np.argmax(delta[T - 1, :])
        # iterate from t =  T-2 to 0 and update Q[t] each time
        for t in reversed(range(T - 1)):
            Q_star[t] = phi[t + 1, Q_star[t + 1]]

        return max(delta[T - 1]), list(Q_star)


"""
Name: Talal Shafei

"""
import numpy as np
import pickle
from sklearn.metrics import accuracy_score
from sklearn.model_selection import StratifiedKFold
from Distance import Distance
from Knn import KNN

dataset, labels = pickle.load(open("../data/part1_dataset.data", "rb"))


"""
  Hyperparameters to test:
  similarity function: Cosine, Minkowski, Mahalanobis
  K: 5, 10, 30

  Total configurations: 9

  I am not doing search over the P value in Minkowski so it will behave like Euclidian distance
  since P default value is 2

"""

# will hold 2D list, each row will take a different combination (similarity function, K_value)
hyperparameter_combinations = [[x, y] for x in [Distance.calculateCosineDistance,
                                                Distance.calculateMinkowskiDistance, Distance.calculateMahalanobisDistance] for y in [5, 10, 30]]

similarity_funs = {Distance.calculateCosineDistance: "Cosine",
                   Distance.calculateMinkowskiDistance: "Minkowski", Distance. calculateMahalanobisDistance: "Mahalanobis"}


# each row will include mean accuracy, and confidence intervals (lower, upper)
stats_arr = np.zeros((9, 3))

# holds the accuracy values of the 10 folds
acc_folds = []

# holds the mean value of the acc_folds on 5 iterations
acc_list = []

for i, (distance_fun, K_value) in enumerate(hyperparameter_combinations):

    acc_list = []

    for j in range(5):

        # setting shuffle to true to shuffle the data before splitting
        sk = StratifiedKFold(10, shuffle=True)

        # clear acc_folds
        acc_folds = []

        for train_index, test_index in sk.split(dataset, labels):

            # dividing the training and the validation datasets for the current fold
            X_train, y_train = dataset[train_index], labels[train_index]
            X_test, y_test = dataset[test_index], labels[test_index]

            # we check if we need to pass Similarity function parameter
            # and create the model
            if distance_fun != Distance.calculateMahalanobisDistance:
                model = KNN(X_train, y_train, distance_fun, K=K_value)
            else:
                S_1 = np.linalg.inv(np.cov(X_train.T))
                model = KNN(X_train, y_train, distance_fun, S_1, K_value)

            # get model prediction on X_test and append its accuracy to acc_folds
            y_hat = model.predict(X_test)
            acc_folds.append(accuracy_score(y_test, y_hat))

        # appending the accuracy score of the 10 folds
        acc_list.append(np.mean(acc_folds))

    # calculating mean and std of accuracy score of the 5 iterations on a hyperparameter combination
    Mean = np.mean(acc_list)
    Std = np.std(acc_list)

    lower = Mean - 1.96*Std/np.sqrt(5.0)
    upper = Mean + 1.96*Std/np.sqrt(5.0)

    # convert to percent %
    Mean *= 100
    upper *= 100
    lower *= 100

    stats_arr[i][0] = Mean
    stats_arr[i][1] = lower
    stats_arr[i][2] = upper

    print()
    print(f"For hyperparameter configuration {i+1}:")
    print(
        f"Distance Function: {similarity_funs[distance_fun]}, K value: {K_value}")
    print(
        f"Accuracy: {Mean:.3f}, with 95% Confidence Interval [{lower:.3f}, {upper:.3f}]")
    print()


# finding the best hyperparameter combination
best_index = np.argmax(stats_arr, axis=0)[0]
best_accuracy = stats_arr[best_index][0]
best_confidence_interval = stats_arr[best_index][1:]
best_hyper = hyperparameter_combinations[best_index]

print()
print(
    f"Best Hyperparameter corresponds to the hyperparameter combination {best_index+1}:")
print(
    f"Distance function: {similarity_funs[best_hyper[0]]}, and K value: {best_hyper[1]}")
print(
    f"With Mean accuracy: {best_accuracy:.3f}, and 95% Confidence interval [{best_confidence_interval[0]:.3f}, {best_confidence_interval[1]:.3f}] ")
print()

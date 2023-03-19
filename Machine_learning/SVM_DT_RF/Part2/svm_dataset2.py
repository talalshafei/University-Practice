"""
Name: Talal Shafei
"""
import pickle
import numpy as np
from sklearn.svm import SVC
from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import Pipeline
from sklearn.model_selection import GridSearchCV, RepeatedStratifiedKFold

dataset, labels = pickle.load(open("../data/part2_dataset2.data", "rb"))


# printing the results
def print_config_info(i, params, mean_array, std_array, confidence):
    print(f"Configuration {i + 1}: C: {params[i]['svc__C']}, Kernel: {params[i]['svc__kernel']}")
    print(f"Mean Accuracy: {mean_array[i] * 100:.3f}%")
    print(f"Standard Deviation: {std_array[i] * 100:.3f}")
    print(f"Confidence Interval: [{confidence[i][0] * 100:.3f}, {confidence[i][1] * 100:.3f}]")
    print()


# define the Support Vector Machine pipeline
# StandardScaler to scale the training data and the validation data on the
# statistics obtained from the training partitions in the KFold
svc_pipe = Pipeline([
    ('scaler', StandardScaler()),
    ('svc', SVC())
])

# choosing the hyperparameters
hyperparameters = {
    'svc__kernel': ['poly', 'rbf'],
    'svc__C': [1, 5]
}

# define the CV splitter
folds = RepeatedStratifiedKFold(n_splits=10, n_repeats=5)
# define the grid search we will be using and setting the scoring to the accuracy
grid_svc = GridSearchCV(svc_pipe, hyperparameters, scoring='accuracy', cv=folds)

# fit the data
grid_svc.fit(dataset, labels)

# list of the configurations tested
params = grid_svc.cv_results_['params']
# transforming acc_matrix to 2D numpy array to obtain statistics from it
mean_array = grid_svc.cv_results_['mean_test_score']
std_array = grid_svc.cv_results_['std_test_score']
# the square root of the number of accuracy that we took their means
N = len(mean_array)
sqrtN = np.sqrt(N)
# finding the confidence interval fo each configuration
confidence = [(mean_ - 1.96 * std_ / sqrtN, mean_ + 1.96 * std_ / sqrtN) for mean_, std_ in
              zip(mean_array, std_array)]

# getting the best configuration based on the highest mean accuracy score
best_index = grid_svc.best_index_

for i in range(0, N):
    print_config_info(i, params, mean_array, std_array, confidence)

print(f"Best configuration corresponds to index {best_index + 1}")
print_config_info(best_index, params, mean_array, std_array, confidence)

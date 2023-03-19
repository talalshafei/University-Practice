"""
Name: Talal Shafei

I tried to make the code independent of the machine learning algorithms
asked to compare in the assignment, so with little modifications I can re-use
it later on my personal projects

Notes:
    1.  Since the outer and inner folds random state are determined at the beginning of the run
        we don't need to train all the models at once sequentially in the inner loop or the outer loop
        of the nested cross validation, instead we can finish the nested cross validation for the whole model
        then proceed to do it to the next model, because we are
        confident that they will train at the same partitions
        of the data due to the initializing of the random state we did in the beginning of the

    2.  This code will also show - only if verbose was True - the statistics
        for the best parameters obtained from the inner cross-validation
        in total we have number (splits of the outer folds multiplied by the number of repetitions
        of the outer folds) of the best configurations

    3.  As shown below stochastic models params must be in list of dictionaries format, but non-stochastic models
        must be on dictionary format with key prefix, so it can be passed to GridSearchCV with Pipeline

    4.  The code took 2h 8min 55s to run on Google Collab, (most of the time spent on Random Forests)
"""
from DataLoader import DataLoader
import numpy as np
from sklearn.neighbors import KNeighborsClassifier
from sklearn.svm import SVC
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import RepeatedStratifiedKFold, GridSearchCV, cross_validate
from sklearn.preprocessing import MinMaxScaler
from sklearn.pipeline import Pipeline
from sklearn.metrics import accuracy_score, f1_score

# Global Variables
data_path = "../data/credit.data"

dataset, labels = DataLoader.load_credit_with_onehot(data_path)

# defining the keys of the models
keys = ['knn', 'svc', 'dtc', 'rfc']

# defining the names of the model, will be used for print purposes
names = {
    'knn': 'K-Nearest Neighbors',
    'svc': 'Support Vector Machine',
    'dtc': 'Decision Tree',
    'rfc': 'Random Forests'
}

# defining dictionary of the models
models = {
    'knn': KNeighborsClassifier,
    'svc': SVC,
    'dtc': DecisionTreeClassifier,
    'rfc': RandomForestClassifier
}

# Random forest configurations
rfc_params = [{'n_estimators': n_estimators, 'criterion': criterion} for n_estimators in [100, 150] for criterion in
              ['gini', 'entropy']]

# Models parameters to test,
params = {
    'knn': {
        'knn__n_neighbors': [10, 15],
        'knn__metric': ['manhattan', 'cosine']
    },
    'svc': {
        'svc__C': [10, 15],
        'svc__kernel': ['rbf', 'sigmoid']
    },
    'dtc': {
        'dtc__ccp_alpha': [0.01, 0.02],
        'dtc__criterion': ['gini', 'entropy']
    },
    'rfc': rfc_params
}

inner_folds = RepeatedStratifiedKFold(n_splits=5, n_repeats=5, random_state=np.random.randint(1, 1000))
outer_folds = RepeatedStratifiedKFold(n_splits=3, n_repeats=5, random_state=np.random.randint(1, 1000))


# End of Global Variables

# Start of helper functions


def compute_confidence(mean_, std_, N):
    # compute 95% confidence interval
    sqrtN = np.sqrt(N)
    return (mean_ - 1.96 * std_ / sqrtN, mean_ + 1.96 * std_ / sqrtN)


def compute_score(y, y_hat, score_metric):
    # will be used in the manual cross validation for stochastic models
    if score_metric == 'accuracy':
        return accuracy_score(y, y_hat)

    return f1_score(y, y_hat, average='micro')


def fetch_best_param(params_scores):
    # getting the best parameter based on highest mean score
    best_mean = -np.inf
    best_std = 0
    best_param = None
    confidence = (0, 0)

    for param_str, scores_list in params_scores.items():
        mean_ = np.mean(scores_list)

        if mean_ > best_mean:
            best_mean = mean_
            best_std = np.std(scores_list)
            confidence = compute_confidence(best_mean, best_std, len(scores_list))
            best_param = param_str

    return best_mean, best_std, confidence, eval(best_param)


def print_inner_results(key, inner_stats, scoring):
    print()
    print(f'Best Inner results for {names[key]} based on {scoring} metric:')
    for i, stats_dict in enumerate(inner_stats, start=1):
        print(
            f'{i}: {tuple(stats_dict["param"].items())} -> Mean: {stats_dict["mean"]:.3f}, STD: {stats_dict["std"]:.3f}, Confidence Interval [{stats_dict["confidence"][0]:.3f},{stats_dict["confidence"][1]:.3f}]')


def print_model_results(keys, model_stats, scoring='accuracy'):
    print()
    for key in keys:
        print(f'Stats for {names[key]} based on {scoring} metric:')
        print(f'Mean: {model_stats[key]["mean"]:.3f}, STD: {model_stats[key]["std"]:.3f}')
        print(
            f'And 95% confidence interval [{model_stats[key]["confidence"][0]:.3f} ,{model_stats[key]["confidence"][1]:.3f}]')
        print()


def process_validate_output(key, validate_dict, scoring, verbose=True):
    # inner_stats list to keep the statistics of the best configurations in the inner cross validation
    inner_stats = []

    if verbose:
        # process to obtain results from the inner cross_validation
        for estimator in validate_dict['estimator']:
            index = estimator.best_index_
            mean_score = estimator.best_score_
            std_score = estimator.cv_results_['std_test_score'][index]
            confidence = compute_confidence(mean_score, std_score, len(estimator.cv_results_['mean_test_score']))
            best_estimator = estimator.best_params_

            inner_stats.append({
                'param': best_estimator,
                'mean': mean_score,
                'std': std_score,
                'confidence': confidence,
            })

        print_inner_results(key, inner_stats, scoring)

    return validate_dict['test_score']


def nested_cross_val(key, scoring='accuracy', verbose=True):
    scaler = MinMaxScaler(feature_range=(-1, 1))
    model_pipe = Pipeline([
        ('scaler', scaler),
        (key, models[key]())
    ])
    model_grid = GridSearchCV(model_pipe, params[key], scoring=scoring, cv=inner_folds, refit=True)
    val = cross_validate(model_grid, dataset, labels, scoring=scoring, cv=outer_folds, return_estimator=True)
    return process_validate_output(key, val, scoring, verbose)


def nested_cross_val_stochastic(key, scoring='accuracy', verbose=True):
    scaler = MinMaxScaler(feature_range=(-1, 1))

    # inner_stats list to keep the statistics of the best configurations in the inner cross validation
    inner_stats = []
    # to return the scores of the outer validation
    scores_list = []
    # outer cross-validation
    for outer_train, outer_valid in outer_folds.split(dataset, labels):
        X_train_outer, y_train_outer = dataset[outer_train], labels[outer_train]
        X_valid_outer, y_valid_outer = dataset[outer_valid], labels[outer_valid]

        # will be used to make calculation to find the best configuration
        params_scores = dict()

        for inner_train, inner_valid in inner_folds.split(X_train_outer, y_train_outer):

            X_train_inner_scaled, y_train_inner = scaler.fit_transform(X_train_outer[inner_train]), y_train_outer[
                inner_train]
            X_valid_inner_scaled, y_valid_inner = scaler.transform(X_train_outer[inner_valid]), y_train_outer[
                inner_valid]

            # fitting on every configuration
            for param in params[key]:

                param_str = str(param)

                # check if this the first time for the parameter
                if param_str not in params_scores:
                    params_scores[param_str] = []

                # loop for ten iterations to eliminate the randomness of fitting stochastic model
                # and obtain more robust results
                random_scores_list = []
                for _ in range(10):
                    model = models[key](**param)
                    model.fit(X_train_inner_scaled, y_train_inner)

                    predicted = model.predict(X_valid_inner_scaled)
                    random_scores_list.append(compute_score(y_valid_inner, predicted, scoring))

                true_score = np.mean(random_scores_list)
                params_scores[param_str].append(true_score)

        # fetching the best configuration from the dictionary
        best_mean, best_std, confidence, best_param = fetch_best_param(params_scores)
        # append the best configuration and its stats to the inner_stats
        inner_stats.append({
            'param': best_param,
            'mean': best_mean,
            'std': best_std,
            'confidence': confidence
        })

        # scaling the data
        X_train_outer_scaled = scaler.fit_transform(X_train_outer)
        X_valid_outer_scaled = scaler.transform(X_valid_outer)

        # loop for ten iterations to eliminate the randomness of fitting stochastic model
        # and obtain more robust results
        random_scores_list = []
        for _ in range(10):
            model = models[key](**best_param)
            model.fit(X_train_outer_scaled, y_train_outer)

            predicted = model.predict(X_valid_outer_scaled)
            random_scores_list.append(compute_score(y_valid_outer, predicted, scoring))

        true_score = np.mean(random_scores_list)
        scores_list.append(true_score)

    # printing the inner results of the nested cross validation
    if verbose:
        print_inner_results(key, inner_stats, scoring)

    return scores_list


def evaluate_models(keys, scoring, verbose=True):
    model_stats = dict.fromkeys(keys, {})
    for key in keys:
        if key == 'rfc':
            model_scores = nested_cross_val_stochastic('rfc', scoring=scoring, verbose=verbose)
        else:
            model_scores = nested_cross_val(key, scoring=scoring, verbose=verbose)

        # find models stats
        model_mean = np.mean(model_scores)
        model_std = np.std(model_scores)
        confidence = compute_confidence(model_mean, model_std, len(model_scores))

        model_stats[key] = {
            'mean': model_mean,
            'std': model_std,
            'confidence': confidence
        }

    print_model_results(keys, model_stats, scoring)


# End of helper functions


# Compare models on accuracy and F1 score
print("Comparing Models on Accuracy: \n")
evaluate_models(keys, scoring='accuracy')
print()
print("*" * 100)
print("Comparing Models on F1 Score: \n")
evaluate_models(keys, scoring='f1_micro')
print()

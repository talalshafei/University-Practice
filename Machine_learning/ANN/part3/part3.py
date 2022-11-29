"""

this code was executed on Google Colab (CPU runtime)
and based on %%time the entire code took: 5h 55m 35s
the full output can be find in part3_output.txt

"""

import pickle
import numpy as np
import torch.nn as nn
import torch


# we load all the datasets of Part 3
x_train, y_train = pickle.load(open("data/mnist_train.data", "rb"))
x_validation, y_validation = pickle.load(
    open("data/mnist_validation.data", "rb"))
x_test, y_test = pickle.load(open("data/mnist_test.data", "rb"))

x_train = x_train/255.0
x_train = x_train.astype(np.float32)

x_test = x_test / 255.0
x_test = x_test.astype(np.float32)

x_validation = x_validation/255.0
x_validation = x_validation.astype(np.float32)

# and converting them into Pytorch tensors in order to be able to work with Pytorch
x_train = torch.from_numpy(x_train)
y_train = torch.from_numpy(y_train).to(torch.long)

x_validation = torch.from_numpy(x_validation)
y_validation = torch.from_numpy(y_validation).to(torch.long)

x_test = torch.from_numpy(x_test)
y_test = torch.from_numpy(y_test).to(torch.long)

###################################################################################
# Start of my code #


# Early Stop to eliminate the epoch search
# I am used to Keras EarlyStopping callback so the implementation
# looks similar to it (for example variables name patience)
# we will break train loop if the validation loss didn't decrease
# or get got worse (increased) for number of consecutive epochs (assumed 5)

class EarlyStop():
    def __init__(self, patience=5):
        self.patience = patience
        self.counter = 0
        self.previous = np.inf

    def __call__(self, loss):
        if loss < self.previous:
            self.previous = loss
            self.counter = 0

        else:
            self.counter += 1

            if self.counter >= self.patience:
                return True

        return False


# classifier details

class MLPModel(nn.Module):
    # the model will take the number of hidden layers, units and the activation function
    def __init__(self, layers=3, units=128, activation=torch.nn.ReLU):
        super(MLPModel, self).__init__()

        self.layers = layers
        self.units = units
        self.activation_function = activation()
        # softmax will be used when calculating the accuracy
        self.softmax_function = torch.nn.Softmax(dim=1)
        self.loss_function = nn.CrossEntropyLoss()

        self.layer_input = nn.Linear(784, self.units)
        self.hiddens = [nn.Linear(self.units, self.units)
                        for _ in range(self.layers)]
        self.layer_output = nn.Linear(self.units, 10)

    # accuracy metric
    # because the labels are label encoding (1,2,3, etc) there is no need to apply argmax on them
    def acc_score(self, prediction, ground_truth):
        probability = self.softmax_function(prediction)
        return (torch.argmax(probability, dim=1) == ground_truth).float().mean() * 100

    # the forward
    def forward(self, x):

        # first will apply the activation function on the linear transformation of the inputs
        z = self.activation_function(self.layer_input(x))

        # then connecting the models layers
        for i in range(self.layers):
            z = self.activation_function(self.hiddens[i](z))

        # finally conects the output layer without the softmax function
        # because the cross-entropy loss in pytorch already calculates it
        output_layer = self.layer_output(z)

        return output_layer

    # the fit function must take the training data and it labels
    # providing the validation data is optional
    # other parameters have default values
    # if the verbose parameter is set to true we will print everthing in detail
    # otherwise only print what epoch we ended the training loop on due to Earlystopping callback
    def fit(self, training_data, training_labels, validation_data=None, validation_labels=None, learning_rate=1e-3, epochs=250, batch=108, verbose=True):

        # first we set our optimizer
        # and our callbacks EarlyStopping in our case
        optimizer = torch.optim.Adam(self.parameters(), lr=learning_rate)
        early_stopping = EarlyStop(patience=5)

        # mini batche gradient descent algorithm will be applied
        # so used DataLoader to divide my training set into batches
        data_loader = torch.utils.data.DataLoader(
            training_data, batch_size=batch, shuffle=False)
        labels_loader = torch.utils.data.DataLoader(
            training_labels, batch_size=batch, shuffle=False)

        # training loop
        for epoch in range(1, epochs + 1):
            # mini batch loop
            # x will be the trainig batch
            # y will be the labels bacth
            for x, y in zip(data_loader, labels_loader):
                # setting the gradient to zero
                optimizer.zero_grad()
                # applying the forward pass
                predictions = self(x)
                # calculating the loss function
                batch_loss = self.loss_function(predictions, y)
                # solving for the gradient
                batch_loss.backward()
                # applying the update rule
                optimizer.step()

            # evaluating the model on the current epoch
            with torch.no_grad():
              # prediction
                train_predictions = self(training_data)

              # loss
                train_loss = self.loss_function(
                    train_predictions, training_labels)

              # accuracy
                train_accuracy = self.acc_score(
                    train_predictions, training_labels)

                # this part only if a validation set is provided
                if validation_data != None:
                  # prediction
                    validation_predictions = self(validation_data)
                  # loss
                    validation_loss = self.loss_function(
                        validation_predictions, validation_labels)

                  # accuracy
                    validation_accuracy = self.acc_score(
                        validation_predictions, validation_labels)

                    if verbose:
                        print(
                            f"Epoch: {epoch} Training Loss: {train_loss:.5f} - Training Accuracy:{train_accuracy:.2f} - Validation Loss: {validation_loss:.5f} - Validation Accuracy: {validation_accuracy:.2f}")
                    # Early stop callback checking
                    if early_stopping(validation_loss):
                        print(f"Early stop on epoch {epoch}")
                        break
                else:
                    if verbose:
                        print(
                            f"Epoch: {epoch} Training Loss: {train_loss:.5f} - Training Accuracy:{train_accuracy:.2f}")

    # this function check the our trained model on a given Test data
    def evaluate(self, x, y, verbose=True):
        with torch.no_grad():
          # prediction
            preds = self(x)
          # loss
            loss = self.loss_function(preds, y)
          # accuracy
            eval_accuracy = self.acc_score(preds, y)
            if verbose:
                print(f"Loss: {loss:.5f} - Accuracy:{eval_accuracy:.2f}")
            return eval_accuracy


########## Grid Search details ############

"""
the hyperparameters [the values that will be tested]:

Required:
  number of hidden layers [3, 7]
  number of neurons       [128, 256]
  learning rates          [1e-3, 5e-3]
  activation functions    [torch.nn.ReLU, torch.nn.LeakyReLU()]

Optional:
  minibatch               [108, 463]

Note:
  Epochs Eliminated because of the use or EaelyStopping callback

Result:
  32 hyperparameter configuations will be tested in total

"""

# dictionary to fetch the name of the activation functions in print statements
activations_funs = {torch.nn.ReLU: "Relu", torch.nn.LeakyReLU: "Leaky Relu"}

# using list comprehension to get all the hyperparameter combinations
hyperparameter_configurations = [(x1, x2, x3, x4, x5) for x1 in [3, 7] for x2 in [128, 256] for x3 in [
    1e-3, 5e-3] for x4 in [torch.nn.ReLU, torch.nn.LeakyReLU]for x5 in [108, 463]]

# will store the Mean accuracy, standard deviation and confience intervals
# of the hyperparameter configurations above
configurations_statistics = np.zeros(shape=(32, 4), dtype=np.float32)

# the outer loop will run for 32 times (total number of hyperparameter combinations)
# the inner loop will run for 10 times, number of training on each hyperparameter combination
# accuracy list will collect the accuracy after each iteration in the inner loop so we can
# get it's mean and standard deviation then calculates the confidence interval
for i, (layers, neurons, learning_rate, activation, batch_size) in enumerate(hyperparameter_configurations):
    accuracy_list = []
    for _ in range(10):
        # creating the model
        model = MLPModel(layers, neurons, activation)
        # training it on the hyperparameter combination
        model.fit(x_train, y_train, x_validation, y_validation,
                  learning_rate, epochs=250, batch=batch_size, verbose=False)
        # getting the accuracy on the validation list
        accuracy_list.append(model.evaluate(
            x_validation, y_validation, verbose=False))

    # casting our list to numpy array to the math on it
    arr = np.array(accuracy_list, np.float32)
    Mean = np.mean(arr)
    Std = np.std(arr)
    Lower_bound = Mean - (1.96*Std/np.sqrt(10))
    Upper_bound = Mean + (1.96*Std/np.sqrt(10))

    configurations_statistics[i, 0] = Mean
    configurations_statistics[i, 1] = Std
    configurations_statistics[i, 2] = Lower_bound
    configurations_statistics[i, 3] = Upper_bound

    # Results
    print()
    print(f"For Hyper parameter configuration {i+1} out of 32: ")
    print(
        f"Hidden layers {layers}, Neurons {neurons}, Learning Rate {learning_rate}, {activations_funs[activation]} Activation Function, Batch_size {batch_size}")
    print(
        f"The Statistics are Mean: {Mean:.3f}, Standard Deviation: {Std:.3f}, Confidence Interval: [{Lower_bound:.3f},{Upper_bound:.3f}]")
    print()


# getting the best hyperparameter combination
# by finding the best mean accuracy
index = np.argmax(configurations_statistics, axis=0)[0]
best_hyperparameters = hyperparameter_configurations[index]

layers, neurons, learning_rate, activation, batch_size = best_hyperparameters
print()
print(f"Best Hyperparameter combination:")
print(f"- Number of hidden Layers: {layers}")
print(f"- Number of Neurons: {neurons}")
print(f"- Learning Rate: {learning_rate}")
print(f"- Activation: {activations_funs[activation]}")
print(f"- Batch Size: {batch_size}")
print()

# combining the training set and the validation set
X = np.concatenate((x_train, x_validation), axis=0)
y = np.concatenate((y_train, y_validation), 0)

# shuffle them
indices = list(range(len(X)))
np.random.shuffle(indices)
X = np.array([X[i] for i in indices], dtype=np.float32)
y = np.array([y[i] for i in indices], dtype=np.float32)

# casting them into tensors
X = torch.from_numpy(X)
y = torch.from_numpy(y).to(torch.long)

# now training the model on the best hyperparameter 10 times and getting the statistics on the test set
# same steps as above but without outer loop and we are not saving the results in the configuration matrix
accuracy_test_list = []
for _ in range(10):
    model = MLPModel(layers, neurons, activation)
    model.fit(X, y, x_test, y_test, learning_rate=learning_rate,
              epochs=250, batch=batch_size, verbose=False)
    accuracy_test_list.append(model.evaluate(x_test, y_test, verbose=False))

arr = np.array(accuracy_test_list, np.float32)
Mean = np.mean(arr)
Std = np.std(arr)

Lower_bound = Mean - (1.96*Std/np.sqrt(10))
Upper_bound = Mean + (1.96*Std/np.sqrt(10))

print()
print("The Statistics on the Test set:")
print(
    f"Mean: {Mean:.3f}, Standard Deviation: {Std:.3f}, Confidence Interval: [{Lower_bound:.3f},{Upper_bound:.3f}]")
print()

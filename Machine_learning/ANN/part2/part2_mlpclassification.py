

import torch
import numpy as np
import pickle
import matplotlib.pyplot as plt


# in this Assignemnt I will consider W an the transposed W in the mathmetical formula so I don't have to take the
# the transpose each time I solve for matmul(W.T,x) and intead it will be matmul(W, x)
# also I will consider the units and the bias as row vectors (1,n)
# so instead of matmul(W,x) it will be matmul(x,W)

# this function returns the neural network output for a given dataset and set of parameters


def forward_pass(w1, b1, w2, b2, input_layer):
    """
    The network consists of 3 inputs, 16 hidden units, and 3 output units
    The activation function of the hidden layer is sigmoid.
    The output layer should apply the softmax function to obtain posterior probability distribution. And the function should return this distribution
    Here you are expected to perform all the required operations for a forward pass over the network with the given dataset
    """

    sigmoid_fun = torch.nn.Sigmoid()
    softmax_fun = torch.nn.Softmax(dim=1)

    hidden_layer = sigmoid_fun(torch.matmul(input_layer, w1) + b1)
    output_layer = softmax_fun(torch.matmul(hidden_layer, w2) + b2)

    return output_layer


# applying torch.argmax to both cause of one-hot encoder representation
def acc_score(preds, ground_truth):
    return (torch.argmax(preds, dim=1) == torch.argmax(ground_truth, dim=1)).float().mean() * 100


# we load all training, validation, and test datasets for the classification task
train_dataset, train_label = pickle.load(
    open("data/part2_classification_train.data", "rb"))
validation_dataset, validation_label = pickle.load(
    open("data/part2_classification_validation.data", "rb"))
test_dataset, test_label = pickle.load(
    open("data/part2_classification_test.data", "rb"))

# when you inspect the training dataset, you are going to see that the class instances are sequential (e.g [1,1,1,1 ... 2,2,2,2,2 ... 3,3,3,3])
# we shuffle the training dataset by preserving instance-label relationship
indices = list(range(len(train_dataset)))
np.random.shuffle(indices)
train_dataset = np.array([train_dataset[i] for i in indices], dtype=np.float32)
train_label = np.array([train_label[i] for i in indices], dtype=np.float32)

# In order to be able to work with Pytorch, all datasets (and labels/ground truth) should be converted into a tensor
# since the datasets are already available as numpy arrays, we simply create tensors from them via torch.from_numpy()
train_dataset = torch.from_numpy(train_dataset)
train_label = torch.from_numpy(train_label)

validation_dataset = torch.from_numpy(validation_dataset.astype(np.float32))
validation_label = torch.from_numpy(validation_label.astype(np.float32))

test_dataset = torch.from_numpy(test_dataset.astype(np.float32))
test_label = torch.from_numpy(test_label.astype(np.float32))

# You are expected to create and initialize the parameters of the network
# Please do not forget to specify requires_grad=True for all parameters since they need to be trainable.

# w1 defines the parameters between the input layer and the hidden layer
# did the definig and the initialization at the same step
# also the shape of w1 is 16*3 so when we multipy it with the input data we will get
# matmul(16*3, 3*1) = 16*1 wich result in the hidden units shape

w1 = torch.from_numpy(np.random.normal(
    0, 1, (3, 16)).astype(np.float32)).requires_grad_(True)

# b defines the bias parameters for the hidden layer
# the shape of b1 is the same as the hiddn units shape
b1 = torch.from_numpy(np.random.normal(
    0, 1, (1, 16)).astype(np.float32)).requires_grad_(True)


# w2 defines the parameters between the hidden layer and the output layer
w2 = torch.from_numpy(np.random.normal(
    0, 1, (16, 3)).astype(np.float32)).requires_grad_(True)


# and finally, b2 defines the bias parameters for the output layer
b2 = torch.from_numpy(np.random.normal(
    0, 1, (1, 3)).astype(np.float32)).requires_grad_(True)


# you are expected to use the stochastic gradient descent optimizer
# w1, b1, w2 and b2 are the trainable parameters of the neural network
# to see faster change in the loss and accuracy adjust the learning rate to 5e-3
optimizer = torch.optim.SGD([w1, b1, w2, b2], lr=0.001)

# define the loss function


def cross_entropy(preds, labels):
    m = float(labels.shape[0])
    loss = -1*torch.sum(labels*torch.log(preds))
    mean = loss / m
    return mean


# These arrays will store the loss values incurred at every training iteration
iteration_array = []
train_loss_array = []
validation_loss_array = []

# We are going to perform the backpropagation algorithm 'ITERATION' times over the training dataset
# After each pass, we are calculating the average/mean cross entropy loss over the validation dataset along with accuracy scores on both datasets.
ITERATION = 15000
for iteration in range(1, ITERATION+1):
    iteration_array.append(iteration)

    # we need to zero all the stored gradient values calculated from the previous backpropagation step.
    optimizer.zero_grad()
    # Using the forward_pass function, we are performing a forward pass over the network with the training data
    train_predictions = forward_pass(w1, b1, w2, b2, train_dataset)
    # here you are expected to calculate the MEAN cross-entropy loss with respect to the network predictions and the training label
    train_mean_crossentropy_loss = cross_entropy(
        train_predictions, train_label)

    train_loss_array.append(train_mean_crossentropy_loss.item())

    # We initiate the gradient calculation procedure to get gradient values with respect to the calculated loss
    train_mean_crossentropy_loss.backward()
    # After the gradient calculation, we update the neural network parameters with the calculated gradients.
    optimizer.step()

    # after each epoch on the training data we are calculating the loss and accuracy scores on the validation dataset
    # with torch.no_grad() disables gradient operations, since during testing the validation dataset, we don't need to perform any gradient operations
    with torch.no_grad():
        # Here you are expected to calculate the accuracy score on the training dataset by using the training labels.
        train_accuracy = acc_score(train_predictions, train_label)

        validation_predictions = forward_pass(
            w1, b1, w2, b2, validation_dataset)

        # Here you are expected to calculate the average/mean cross entropy loss for the validation datasets by using the validation dataset labels.
        validation_mean_crossentropy_loss = cross_entropy(
            validation_predictions, validation_label)

        validation_loss_array.append(validation_mean_crossentropy_loss.item())

        # Similarly, here, you are expected to calculate the accuracy score on the validation dataset
        validation_accuracy = acc_score(
            validation_predictions, validation_label)

    print("Iteration : %d - Train Loss %.4f - Train Accuracy : %.2f - Validation Loss : %.4f Validation Accuracy : %.2f" %
          (iteration+1, train_mean_crossentropy_loss.item(), train_accuracy, validation_mean_crossentropy_loss.item(), validation_accuracy))


# after completing the training, we calculate our network's accuracy score on the test dataset...
# Again, here we don't need to perform any gradient-related operations, so we are using torch.no_grad() function.
with torch.no_grad():
    test_predictions = forward_pass(w1, b1, w2, b2, test_dataset)
    # Here you are expected to calculate the network accuracy score on the test dataset...
    test_accuracy = acc_score(test_predictions, test_label)
    # removed the .item() from the print statement below
    print("Test accuracy : %.2f" % (test_accuracy))

# We plot the loss versus iteration graph for both datasets (training and validation)
plt.plot(iteration_array, train_loss_array, label="Train Loss")
plt.plot(iteration_array, validation_loss_array, label="Validation Loss")
plt.legend()
plt.show()

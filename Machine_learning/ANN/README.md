# Part2
Implemented two neural network architectures for one classification task
and one regression task, respectively, by using the basic operations of Pytorch (no high-level features
such as custom modules, or Pytorch modules). The classification task of this part involves
learning to predict class labels (out of 3 classes) for a given unseen data sample whereas the regression
task requires learning to predict a single numeric value.

* For the classification task, I implemented a multilayer perceptron which
consists of 1 input layer (with 3 units), 1 hidden layer (with 16 units), and 1 output layer (with 3 units).
The hidden layer is expected to apply the sigmoid function and the output should apply the softmax
function, since it is a multiclass classification task, the network is trained with the
cross-entropy loss function.

* For the regression task, Implement a multilayer perceptron which consists of 1
input layer (2 units), 1 hidden layer (32 units), and 1 output layer (with 1 unit). The hidden layer is
expected to apply the sigmoid function. The output layer does not apply any activation function. The
output yields a single numeric value for a given data sample. The network is trained with the squared
error

# Part3
In this part, performed an extensive hyperparameter (Grid Search) search for the MLP algorithm by utilizing a validation dataset.

* This part asks implement an MLP training code for a classification task and prepare
a report about the training setting, procedures you have considered, and the
decisions made throughout the process as well as the results attained.
* During parameter search, each hyperparameter configuration
should be executed 10 times and their confidence interval scores for the accuracy metric should
be reported

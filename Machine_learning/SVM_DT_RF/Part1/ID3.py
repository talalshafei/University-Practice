"""
Name: Talal Shafei
ID: 2542371
"""
import numpy as np


# In the decision tree, non-leaf nodes are going to be represented via TreeNode
class TreeNode:
    def __init__(self, attribute):
        self.attribute = attribute
        # dictionary, k: subtree, key (k) an attribute value, value is either TreeNode or TreeLeafNode
        self.subtrees = {}


# In the decision tree, leaf nodes are going to be represented via TreeLeafNode
class TreeLeafNode:
    def __init__(self, data, label):
        self.data = data
        self.labels = label


class DecisionTree:
    def __init__(self, dataset: list, labels, features, criterion="information gain"):
        """
        :param dataset: array of data instances, each data instance is represented via a Python array
        :param labels: array of the labels of the data instances
        :param features: the array that stores the name of each feature dimension
        :param criterion: depending on which criterion
               ("information gain" or "gain ratio") the splits are to be performed
        """
        self.dataset = dataset
        self.labels = labels
        self.features = features
        self.criterion = criterion
        # it keeps the root node of the decision tree
        self.root = None

    def fetch_attr_info(self, attribute, dataset):
        # fetch the attribute index from the features list
        attr_index = self.features.index(attribute)
        # get the attribute column
        attr_column = np.array(dataset)[:, attr_index]
        # get the unique set values of the attribute
        attr_values = set(attr_column)

        return attr_index, attr_column, attr_values

    def calculate_entropy__(self, labels):
        """
        :param labels: array of the labels of the data instances
        :return: calculated entropy value for the given dataset
        """
        # initialize the entropy to zero
        entropy_value = 0.0
        # declare the cardinality of the labels
        length = len(labels)

        # dictionary to hold the classes as a key and their count as a value
        labels_count = dict()
        for label in labels:
            # new class
            if label not in labels_count:
                labels_count[label] = 0
            # increment count the current class
            labels_count[label] += 1

        # find the probability of each class by count/cardinality
        for count in labels_count.values():
            p = count / length
            # the entropy formula
            entropy_value -= p * np.log2(p)

        return entropy_value

    def calculate_average_entropy__(self, dataset, labels, attribute):
        """
        :param dataset: array of the data instances on which an average entropy value is calculated
        :param labels: array of the labels of those data instances
        :param attribute: for which attribute an average entropy value is going to be calculated...
        :return: the calculated average entropy value for the given attribute
        """
        # initialize average entropy to 0
        average_entropy = 0.0
        # declare the cardinality of the labels
        length = len(labels)

        # get the attribute info
        _, attr_column, attr_values = self.fetch_attr_info(attribute, dataset)

        # now we find labels subsets for each value
        for value in attr_values:
            labels_subset = []
            for i, label in enumerate(labels):
                if attr_column[i] == value:
                    labels_subset.append(label)

            # average entropy formula
            average_entropy += self.calculate_entropy__(labels_subset) * len(labels_subset) / length

        return average_entropy

    def calculate_information_gain__(self, dataset, labels, attribute):
        """
        :param dataset: array of the data instances on which an information gain score is going to be calculated
        :param labels: array of the labels of those data instances
        :param attribute: for which attribute the information gain score is going to be calculated...
        :return: the calculated information gain score
        """

        # dataset entropy minus the average entropy of the attribute
        information_gain = self.calculate_entropy__(labels) - self.calculate_average_entropy__(dataset, labels,
                                                                                               attribute)

        return information_gain

    def calculate_intrinsic_information__(self, dataset, labels, attribute):
        """
        :param dataset: array of data instances on which an intrinsic information score is going to be calculated
        :param labels: array of the labels of those data instances
        :param attribute: for which attribute the intrinsic information score is going to be calculated...
        :return: the calculated intrinsic information score
        """

        intrinsic_info = 0.0
        length = len(labels)

        # get attribute info
        _, attr_column, attr_values = self.fetch_attr_info(attribute, dataset)

        # find the count of each value in attr_column
        for value in attr_values:
            subset_len = np.sum(attr_column == value)
            p = subset_len / length

            intrinsic_info -= p * np.log2(p)

        return intrinsic_info

    def calculate_gain_ratio__(self, dataset, labels, attribute):
        """
        :param dataset: array of data instances with which a gain ratio is going to be calculated
        :param labels: array of labels of those instances
        :param attribute: for which attribute the gain ratio score is going to be calculated...
        :return: the calculated gain ratio score
        """
        gain = self.calculate_information_gain__(dataset, labels, attribute)
        intrinsic = self.calculate_intrinsic_information__(dataset, labels, attribute)

        return gain / intrinsic

    def ID3__(self, dataset, labels, used_attributes):
        """
        Recursive function for ID3 algorithm
        :param dataset: data instances falling under the current  tree node
        :param labels: labels of those instances
        :param used_attributes: while recursively constructing the tree,
               already used labels should be stored in used_attributes
        :return: it returns a created non-leaf node or a created leaf node
        """
        # classes in labels
        classes = set(labels)
        # in the trivial case if we have one class return leaf node
        if len(classes) == 1:
            return TreeLeafNode(dataset, labels)

        # if there are no more features left to split on return a leaf node with the rest of the data
        if len(self.features) == len(used_attributes):
            return TreeLeafNode(dataset, labels)

        # create a new list of unused features
        unused_attributes = list(set(self.features).difference(used_attributes))

        # choosing the best attribute to split on based on the criterion
        if self.criterion == "information gain":
            best_attr = max(unused_attributes,
                            key=lambda attribute: self.calculate_information_gain__(dataset, labels, attribute))

        elif self.criterion == "gain ratio":
            best_attr = max(unused_attributes,
                            key=lambda attribute: self.calculate_gain_ratio__(dataset, labels, attribute))

        else:
            raise ValueError('The criterion value should be either "information gain" or "gain ratio" ')

        # add the best attribute to the used attributes
        # because in categorical attribute the spilt exhausts all the information on that attribute
        used_attributes.append(best_attr)

        # creating a tree on the best attribute
        tree = TreeNode(best_attr)

        # getting the index of the best attribute and the unique values of the best attribute
        best_attr_index, _, best_attr_values = self.fetch_attr_info(best_attr, dataset)

        # creating a subtree for each value of the best attribute
        for value in best_attr_values:
            # new dataset and new labels for each value of the best attribute
            new_dataset = []
            new_labels = []
            for i, data_instance in enumerate(dataset):
                if data_instance[best_attr_index] == value:
                    new_dataset.append(data_instance)
                    new_labels.append(labels[i])

            # creating subtree and attach it to the tree
            subtree = self.ID3__(new_dataset, new_labels, used_attributes)
            tree.subtrees[value] = subtree

        return tree

    def predict(self, x):
        """
        :param: x a data instance, 1 dimensional Python array
        :return: predicted label of x

        If a leaf node contains multiple labels in it, the majority label should be returned as the predicted label
        """
        # start at the root node
        current_node = self.root

        # traversing the tree branches until we reach a leaf node
        while isinstance(current_node, TreeNode):
            # get the index of the attribute that the current node is split on
            attr_index = self.features.index(current_node.attribute)

            # get the value of the attribute from x to decide which branch to go to
            attr_value = x[attr_index]

            # follow the branch by updating the current node
            current_node = current_node.subtrees[attr_value]

        # after reaching a leaf node we will make the prediction based on the majority class in the leaf
        labels = current_node.labels
        classes = set(labels)

        predicted_label = max(classes, key=labels.count)

        return predicted_label

    def train(self):
        self.root = self.ID3__(self.dataset, self.labels, [])
        print("Training completed")

import numpy as np


def softmax(x):
    """ softmax function """

    # assert(len(x.shape) > 1, "dimension must be larger than 1")
    # print(np.max(x, axis = 1, keepdims = True)) # axis = 1, 行

    x = np.exp(x) / np.sum(np.exp(x), axis=-1, keepdims=True)

    return x

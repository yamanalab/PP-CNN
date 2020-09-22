from keras import backend as K
import matplotlib.pyplot as plt
import numpy as np


def square(x):
    return x ** 2


def swish(x):
    return x * K.sigmoid(x)


def mish(x):
    return x * K.tanh(K.log(1 + K.exp(x)))


# Swish approximated by least squares (x range: [-3, 3])
def swish_rg3_deg4(x):
    return 0.0108 + 0.5 * x + 0.2212 * x**2 - 0.008248 * x**4


# Swish approximated by least squares (x range: [-4, 4])
def swish_rg4_deg4(x):
    return 0.03347 + 0.5 * x + 0.19566 * x**2 - 0.005075 * x**4


# Swish approximated by least squares (x range: [-5, 5])
def swish_rg5_deg4(x):
    return 0.07066 + 0.5 * x + 0.17003 * x**2 - 0.00315 * x**4


# Swish approximated by least squares (x range: [-6, 6])
def swish_rg6_deg4(x):
    return 0.1198 + 0.5 * x + 0.1473 * x**2 - 0.002012 * x**4


# Swish approximated by least squares (x range: [-7, 7])
def swish_rg7_deg4(x):
    return 0.1773 + 0.5 * x + 0.1282 * x**2 - 0.001328 * x**4


# Swish approximated by least squares (x range: [-8, 8])
def swish_rg8_deg4(x):
    return 0.2401 + 0.5 * x + 0.11257 * x**2 - 0.000908 * x**4


# Mish approximated by least squares (x range: [-4, 4])
def mish_rg4_deg4(x):
    return 0.06021 + 0.565775*x + 0.21051*x**2 - 0.004142*x**3 - 0.00609*x**4


# Mish approximated by least squares (x range: [-5, 5])
def mish_rg5_deg4(x):
    return 0.1104 + 0.5495*x + 0.17573*x**2 - 0.0022355*x**3 - 0.00346443*x**4


# Mish approximated by least squares (x range: [-6, 6])
def mish_rg6_deg4(x):
    return 0.169 + 0.53663*x + 0.148529*x**2 - 0.001277*x**3 - 0.002096*x**4


def plot_training_history(history, path):
    fig, (acc_ax, loss_ax) = plt.subplots(nrows=2, constrained_layout=True)

    acc_ax.plot(history['acc'])
    acc_ax.plot(history['val_acc'])
    acc_ax.set_title('Model Accuracy')
    acc_ax.set_ylabel('Accuracy')
    acc_ax.set_xlabel('Epoch')
    acc_ax.set_ylim(0, 1.0)
    acc_ax.legend(['training', 'validation'], loc='upper left')
    acc_ax.grid()

    loss_ax.plot(history['loss'])
    loss_ax.plot(history['val_loss'])
    loss_ax.set_title('Model Loss')
    loss_ax.set_ylabel('Loss')
    loss_ax.set_xlabel('Epoch')
    loss_ax.set_ylim(0, 5.0)
    loss_ax.legend(['training', 'validation'], loc='upper left')
    loss_ax.grid()

    fig.savefig(path, dpi=300)

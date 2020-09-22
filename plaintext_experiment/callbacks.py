from tensorflow import keras


class EpochLog(keras.callbacks.Callback):
    def __init__(self, filepath):
        self.filepath = filepath

    def on_train_begin(self, logs={}):
        with open(self.filepath, mode='a') as f:
            f.write("prams: {}\n\n".format(self.params))

    def on_epoch_end(self, epoch, logs={}):
        with open(self.filepath, 'a') as f:
            f.write(
                "epoch: {}\n\tloss: {}, acc: {},\n\tval_loss: {}, val_acc: {}\n"
                .format(epoch, logs['loss'], logs['acc'], logs['val_loss'], logs['val_acc'])
            )

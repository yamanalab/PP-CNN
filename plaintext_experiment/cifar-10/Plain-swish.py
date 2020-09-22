import keras
from keras.models import model_from_json, Sequential
from keras.layers import Conv2D, AveragePooling2D, Dense, Flatten, Activation, BatchNormalization, GlobalAveragePooling2D
from keras.callbacks import EarlyStopping, ReduceLROnPlateau, ModelCheckpoint
from keras.utils import get_custom_objects, CustomObjectScope
from keras.preprocessing.image import ImageDataGenerator
from keras.datasets import cifar10

import os
import sys
import argparse
import json
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split

sys.path.append('..')
import functions
from callbacks import EpochLog
from my_json_encoder import MyJsonEncoder

parser = argparse.ArgumentParser()
parser.add_argument('--bn', action='store_true')
parser.add_argument('--gap', action='store_true')
parser.add_argument('--full', action='store_true')
parser.add_argument('--da', action='store_true')
args = parser.parse_args()
bn_enabled = args.bn
gap_enabled = args.gap
full_enabled = args.full
da_enabled = args.da

# Register custom activation function
get_custom_objects().update({'swish': functions.swish})

# Declare constants
CUR_DIR = os.path.dirname(os.path.abspath(__file__))
BASE_FILE_NAME = os.path.splitext(os.path.basename(__file__))[0]
if bn_enabled:
    BASE_FILE_NAME+='-BN'
if gap_enabled:
    BASE_FILE_NAME+='-GAP'
if full_enabled:
    BASE_FILE_NAME+='-full'
if da_enabled:
    BASE_FILE_NAME+='-DA'
LOG_FILE_PATH = f'{CUR_DIR}/epoch_logs/{BASE_FILE_NAME}_log.txt'
# MODEL_FILE_PATH = f'{CUR_DIR}/saved_models/{BASE_FILE_NAME}_model.h5'
MODEL_WEIGHTS_PATH = f'{CUR_DIR}/saved_models/{BASE_FILE_NAME}_weights.h5'
MODEL_STRUCTURE_PATH = f'{CUR_DIR}/saved_models/{BASE_FILE_NAME}_structure.json'
HISTORY_FILE_PATH = f'{CUR_DIR}/histories/{BASE_FILE_NAME}_history.json'
HISTORY_GRAPH_PATH = f'{CUR_DIR}/graphs/{BASE_FILE_NAME}_history.png'

BATCH_SIZE = 256
NUM_CLASSES = 10
EPOCHS = 150
LABELS = ('airplane', 'mobile', 'bird', 'cat', 'deer',
          'dog', 'frog', 'horse', 'ship', 'truck')


def preprocess_input_image(train_input, test_input):
    # Normalization
    train_input = train_input.astype('float32')
    test_input = test_input.astype('float32')
    train_input /= 255.0
    test_input /= 255.0

    return train_input, test_input


def cifar10_model(input_shape):
    model = Sequential()

    model.add(Conv2D(32, (3, 3), padding='same', input_shape=input_shape))
    if bn_enabled:
        model.add(BatchNormalization())
    model.add(Activation('swish'))
    model.add(AveragePooling2D(pool_size=(2, 2)))

    model.add(Conv2D(64, (3, 3), padding='same'))
    if bn_enabled:
        model.add(BatchNormalization())
    model.add(Activation('swish'))
    model.add(AveragePooling2D(pool_size=(2, 2)))

    model.add(Conv2D(128, (3, 3), padding='same'))
    if bn_enabled:
        model.add(BatchNormalization())
    model.add(Activation('swish'))
    model.add(AveragePooling2D(pool_size=(2, 2)))

    if gap_enabled:
        model.add(GlobalAveragePooling2D())
    else:
        model.add(Flatten())
    model.add(Dense(256))
    if full_enabled:
        model.add(BatchNormalization())
        model.add(Activation('swish'))
    model.add(Dense(NUM_CLASSES, activation='softmax'))

    return model


def main():
    if os.path.exists(LOG_FILE_PATH):
        os.remove(LOG_FILE_PATH)

    # Load cifar10 data
    (x_train, y_train), (x_test, y_test) = cifar10.load_data()

    x_train, x_test = preprocess_input_image(x_train, x_test)
    input_shape = x_train[0].shape

    print('input shape:', input_shape)
    print(x_train.shape[0], 'train samples')
    print(x_test.shape[0], 'test samples')

    # Convert class vectors to binary class metrices (One Hot Encoding)
    y_train = keras.utils.to_categorical(y_train, NUM_CLASSES)
    y_test = keras.utils.to_categorical(y_test, NUM_CLASSES)

    # Split data for training and validation
    if da_enabled:
        X_train, X_val, Y_train, Y_val = train_test_split(x_train, y_train, test_size=0.1, random_state=42)

    # Define CNN model
    model = cifar10_model(input_shape)
    # Conpile the model
    model.compile(loss='categorical_crossentropy',
                  optimizer=keras.optimizers.Adam(),
                  metrics=['accuracy'])

    print(model.summary())
    with open(LOG_FILE_PATH, 'a') as f:
        model.summary(print_fn=lambda x: f.write(x + "\r\n"))
        f.write("\n\n")

    model_structure_json = model.to_json()
    with open(MODEL_STRUCTURE_PATH, 'w') as f:
        f.write(model_structure_json)

    # Define callbacks
    # early_stopping = EarlyStopping(monitor='val_loss', patience=10, verbose=1)
    learning_rate_reduction = ReduceLROnPlateau(monitor='val_loss',
                                                patience=3,
                                                verbose=1,
                                                factor=0.75,
                                                min_lr=1e-5)
    model_checkpoint = ModelCheckpoint(MODEL_WEIGHTS_PATH,
                                       monitor='val_loss',
                                       save_best_only=True,
                                       save_weights_only=True)
    epoch_log = EpochLog(LOG_FILE_PATH)

    callbacks = [
        # early_stopping,
        learning_rate_reduction,
        model_checkpoint,
        epoch_log
    ]

    if da_enabled:
        # Data augumentation
        datagen = ImageDataGenerator(
            rotation_range=20,
            zoom_range=0.2,
            width_shift_range=0.2,
            height_shift_range=0.2,
            horizontal_flip=True,
            vertical_flip=False
        )

        steps_per_epoch = X_train.shape[0] // BATCH_SIZE

        # Train model
        history = model.fit_generator(datagen.flow(X_train, Y_train, batch_size=BATCH_SIZE),
                                    epochs=EPOCHS,
                                    steps_per_epoch=steps_per_epoch,
                                    validation_data=(X_val, Y_val),
                                    verbose=1,
                                    callbacks=callbacks)
    else:
        history = model.fit(x_train, y_train,
                            batch_size=BATCH_SIZE,
                            epochs=EPOCHS,
                            verbose=1,
                            validation_split=0.1,
                            callbacks=callbacks)

    # Save history
    with open(HISTORY_FILE_PATH, 'w') as f:
        json.dump(history.history, f, cls=MyJsonEncoder)

    # Visualize training history
    functions.plot_training_history(history.history, HISTORY_GRAPH_PATH)

    # Load best model
    with CustomObjectScope({'swish': functions.swish}):
        with open(MODEL_STRUCTURE_PATH, 'r') as f:
            model_structure_json = f.read()
        best_model = model_from_json(model_structure_json)
        best_model.load_weights(MODEL_WEIGHTS_PATH)
        best_model.compile(loss='categorical_crossentropy',
                           optimizer=keras.optimizers.Adam(),
                           metrics=['accuracy'])

    # Evaluate model
    test_loss, test_acc = best_model.evaluate(x_test, y_test, verbose=1)

    print('Test loss:', test_loss)
    print('Test accuracy:', test_acc)
    with open(LOG_FILE_PATH, 'a') as f:
        f.write("\n\nTest loss: {}, Test accuracy: {}\n".format(
            test_loss, test_acc))

    # Predict for test samples
    preds = best_model.predict(x_test, verbose=1)
    print("Prediction result:")
    print(preds)


if __name__ == '__main__':
    main()

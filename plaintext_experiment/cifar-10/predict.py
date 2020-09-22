import os
import sys
import argparse
import json
import time
import keras
from keras.datasets import cifar10
from keras.models import model_from_json
from keras.utils import get_custom_objects, CustomObjectScope

sys.path.append('..')
import functions

parser = argparse.ArgumentParser()
parser.add_argument('model_name')
args = parser.parse_args()
model_name = args.model_name

CUR_DIR = os.path.dirname(os.path.abspath(__file__))
MODEL_WEIGHTS_PATH = f'{CUR_DIR}/saved_models/{model_name}_weights.h5'
MODEL_STRUCTURE_PATH = f'{CUR_DIR}/saved_models/{model_name}_structure.json'

os.environ['CUDA_VISIBLE_DEVICES'] = '-1'

if 'HCNN' in model_name:
    get_custom_objects().update({'square': functions.square})
elif 'CKKS-swish_rg4_deg4' in model_name:
    get_custom_objects().update({'swish_rg4_deg4': functions.swish_rg4_deg4})
elif 'CKKS-swish_rg6_deg4' in model_name:
    get_custom_objects().update({'swish_rg6_deg4': functions.swish_rg6_deg4})
elif 'swish' in model_name:
    get_custom_objects().update({'swish': functions.swish})


def preprocess_input_image(train_input, test_input):
    # Normalization
    train_input = train_input.astype('float32')
    test_input = test_input.astype('float32')
    train_input /= 255.0
    test_input /= 255.0

    return train_input, test_input


def main():
    (x_train, y_train), (x_test, y_test) = cifar10.load_data()

    _, x_test = preprocess_input_image(x_train, x_test)
    print(x_test.shape[0], 'test samples')

    # Load model
    if 'HCNN' in model_name:
        with CustomObjectScope({'square': functions.square}):
            with open(MODEL_STRUCTURE_PATH, 'r') as f:
                model_structure_json = f.read()
            model = model_from_json(model_structure_json)
            model.load_weights(MODEL_WEIGHTS_PATH)
    elif 'CKKS-swish_rg4_deg4' in model_name:
        with CustomObjectScope({'swish_rg4_deg4': functions.swish_rg4_deg4}):
            with open(MODEL_STRUCTURE_PATH, 'r') as f:
                model_structure_json = f.read()
            model = model_from_json(model_structure_json)
            model.load_weights(MODEL_WEIGHTS_PATH)
    elif 'CKKS-swish_rg6_deg4' in model_name:
        with CustomObjectScope({'swish_rg6_deg4': functions.swish_rg6_deg4}):
            with open(MODEL_STRUCTURE_PATH, 'r') as f:
                model_structure_json = f.read()
            model = model_from_json(model_structure_json)
            model.load_weights(MODEL_WEIGHTS_PATH)
    elif 'swish' in model_name:
        with CustomObjectScope({'swish': functions.swish}):
            with open(MODEL_STRUCTURE_PATH, 'r') as f:
                model_structure_json = f.read()
            model = model_from_json(model_structure_json)
            model.load_weights(MODEL_WEIGHTS_PATH)
    else:
        with open(MODEL_STRUCTURE_PATH, 'r') as f:
            model_structure_json = f.read()
        model = model_from_json(model_structure_json)
        model.load_weights(MODEL_WEIGHTS_PATH)

    start = time.time()
    preds = model.predict(x_test, verbose=1)
    elapsed_time = time.time() - start
    print()
    print(f'elapsed_time: {elapsed_time} s')


if __name__ == '__main__':
    main()

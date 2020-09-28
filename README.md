# PP-CNN
Privacy Preserving CNN Inference over Homomorphic Encryption

## Directory structure
```
pp-cnn
├── plaintext_experiment      # Train model (Python)
│   ├── mnist
│   │     ├── *.py
│   │     └── saved_models  # Model structure(.json) and weights(.h5)
│   ├── cifar-10
│   　     ├── *.py
│   　     └── saved_models  # Model structure(.json) and weights(.h5)
│
│
├── pp_cnn                    # Secure inference (C++)
　   ├── datasets
　   │     ├── cifar-10
　   │     └── mnist
　   ├── include              # Header files for library
　   ├── secrets              # Store SEAL parameter and key files
　   └── src
　        ├── main.cpp
　        ├── setup.cpp
　        ├── cnn
　        └── utils
```

## Requirement
- GCC (7.4.0)
- CMake (>=3.15)
- [SEAL (3.4.4)](https://github.com/microsoft/SEAL/tree/3.4.4)
- Boost (Use `boost::multi_array` & `boost::program_options`)
- HDF5
  - Require zlib & szip

[Model training] (Python)
- Keras(2.0.8)
- tensorflow-gpu(1.4.0)

## Dataset
- MNIST (http://yann.lecun.com/exdb/mnist/)
- CIFAR-10 (https://www.cs.toronto.edu/~kriz/cifar.html)

### Download MNIST dataset in `pp_cnn/datasets/mnist/`
```
$ mkdir -p pp_cnn/datasets/mnist && cd pp_cnn/datasets/mnist
$ wget http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz
$ wget http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz
$ wget http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz
$ wget http://yann.lecun.com/exdb/mnist/t10k-labels-idx1-ubyte.gz
$ gunzip *.gz
```

### Download CIFAR-10 dataset in `pp_cnn/datasets/cifar-10/`
```
$ mkdir -p pp_cnn/datasets/cifar-10 && cd pp_cnn/datasets/cifar-10
$ wget https://www.cs.toronto.edu/~kriz/cifar-10-binary.tar.gz
$ tar zxvf cifar-10-binary.tar.gz
$ mv cifar-10-batches-bin/* ./ && rmdir cifar-10-batches-bin/
```

## Train model
Model training code (Python) is in `plaintext_experiment/`

ex) MNIST dataset
```
$ cd plaintext_experiment/mnist
$ python HCNN.py --da
```

## Build pp_cnn
You can build by executing the following commands:
```
$ cd pp_cnn/src
$ mkdir build && cd build
$ cmake ..
$ make
```

## Run
1. Setup SEAL setting (setup.cpp)
```
$ cd pp_cnn/bin
$ ./setup -P 15 -L 5
```
2. Secure inference of test data (main.cpp)
```
$ cd pp_cnn/bin
$ OMP_NUM_THREADS=xx ./main -D mnist -M HCNN-DA
```

## License
Copyright 2020 Yamana Laboratory, Waseda University Supported by JST CREST Grant Number JPMJCR1503, Japan.

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

# PP-CNN
Privacy preserving CNN inference on homomorphic encryption

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
- CMake(>=3.15)
- [SEAL(3.4.4)](https://github.com/microsoft/SEAL/tree/3.4.4)
- Boost
  - boost::multi_array
  - boost::program_options
- HDF5
  - require zlib & szip

[Model training] (Python)
- Keras(2.0.8)
- tensorflow(1.14.0)
- tensorflow-gpu(1.4.0)

## Install required libraries
Example
```
#/usr/bin/env bash
set -eu

# NOTE: Add following code in .bashrc or .zshrc
# export LD_LIBRARY_PATH="$HOME/.local/lib:$LD_LIBRARY_PATH"

install_cmake () {
  cd ~/downloads && \
  wget https://github.com/Kitware/CMake/releases/download/v3.16.0/cmake-3.16.0.tar.gz && \
  tar xvzf cmake-3.16.0.tar.gz && \
  cd cmake-3.16.0 && \
  mkdir build && \
  cd build && \
  ../bootstrap --prefix="${HOME}/.local" && \
  make && \
  make install
}

install_seal () {
  cd ~/github && \
  git clone git@github.com:microsoft/SEAL.git && \
  cd SEAL && \
  git checkout 3.4.4 && \
  cd native/src && \
  mkdir build && \
  cd build && \
  cmake .. -DCMAKE_INSTALL_PREFIX=~/mylibs && \
  make && \
  make install
}

install_boost () {
  cd ~/downloads && \
  wget https://dl.bintray.com/boostorg/release/1.65.1/source/boost_1_65_1.tar.gz && \
  tar xvzf boost_1_65_1.tar.gz && \
  cd boost_1_65_1 && \
  ./bootstrap.sh --prefix="${HOME}/.local" && \
  ./b2 install -j2 --prefix="${HOME}/.local"
}

install_zlib () {
  cd ~/downloads && \
  wget http://www.zlib.net/zlib-1.2.11.tar.gz && \
  tar xvzf zlib-1.2.11.tar.gz && \
  cd zlib-1.2.11 && \
  ./configure --prefix="${HOME}/.local" && \
  make && \
  make install
}

install_szip () {
  cd ~/downloads && \
  wget https://support.hdfgroup.org/ftp/lib-external/szip/2.1.1/src/szip-2.1.1.tar.gz && \
  tar xvzf szip-2.1.1.tar.gz && \
  cd szip-2.1.1 && \
  ./configure --prefix="${HOME}/.local" && \
  make && \
  make install
}

install_hdf5 () {
  install_zlib
  install_szip
  cd ~/downloads && \
  wget https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.10/hdf5-1.10.0-patch1/bin/linux-centos7-x86_64-gcc485/hdf5-1.10.0-patch1-linux-centos7-x86_64-gcc485-shared.tar.gz && \
  tar xvzf hdf5-1.10.0-patch1-linux-centos7-x86_64-gcc485-shared.tar.gz && \
  cd hdf5-1.10.0-patch1-linux-centos7-x86_64-gcc485-shared && \
  cp include/* ~/.local/include/ && \
  cp lib/* ~/.local/lib/
}

run () {
  [[ -d ~/downloads ]] || mkdir ~/downloads
  [[ -d ~/mylibs ]] || mkdir ~/mylibs
  [[ -d ~/github ]] || mkdir ~/github

  install_cmake
  install_seal
  install_boost
  install_hdf5
}

run
```

## Train model
Model training code (Python) is in `plaintext_experiment/`

ex) MNIST dataset
```
cd plaintext_experiment/mnist
python HCNN.py --da
```

## Build pp_cnn
You can build by executing the following commands:
```
cd pp_cnn/src
mkdir build
cd build
cmake ..
make
```

## Run
1. Setup SEAL setting (setup.cpp)
```
cd pp_cnn/bin
./setup -P 15 -L 5
```
2. Secure inference of test data (main.cpp)
```
cd pp_cnn/bin
OMP_NUM_THREADS=xx ./main -D mnist -M HCNN-DA
```

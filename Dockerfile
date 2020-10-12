FROM centos:7

RUN yum install -y gcc-c++ wget make openssl-devel git bzip2 which mlocate
RUN yum install -y centos-release-scl
RUN yum install -y devtoolset-8-gcc devtoolset-8-gcc-c++
RUN yum install -y xterm

# for Devel (to be removed)
RUN yum install -y emacs

SHELL ["/usr/bin/scl", "enable", "devtoolset-8"]

RUN cd tmp \
    && wget https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.gz \
    && tar xvfz boost_1_74_0.tar.gz \
    && cd boost_1_74_0 \
    && ./bootstrap.sh --without-icu --with-libraries=context,filesystem,graph,iostreams,program_options \
    && ./b2 -j12 link=static,shared runtime-link=shared threading=multi variant=release --layout=tagged --build-dir=../b2gcc --stagedir=stage/gcc stage \
    && ./b2 -j12 --prefix=/usr install \
    && ldconfig

RUN cd tmp \
    && wget https://github.com/Kitware/CMake/releases/download/v3.18.4/cmake-3.18.4.tar.gz \
    && tar xvfz cmake-3.18.4.tar.gz && cd cmake-3.18.4 \
    && ./bootstrap \
    && make -j12 \
    && make install
RUN cd tmp \
    && wget https://support.hdfgroup.org/ftp/lib-external/szip/2.1.1/src/szip-2.1.1.tar.gz \
    && tar xvfz szip-2.1.1.tar.gz && cd szip-2.1.1 \
    && ./configure --prefix=/usr/local \
    && make -j12 \
    && make install
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:/usr/local/lib
RUN cd tmp \
    && wget https://hdf-wordpress-1.s3.amazonaws.com/wp-content/uploads/manual/HDF5/HDF5_1_12_0/source/hdf5-1.12.0.tar.gz \
    && tar xvfz hdf5-1.12.0.tar.gz && cd hdf5-1.12.0 \
    && ./configure --prefix=/usr/local/hdf5 --enable-cxx --with-szlib=/usr/local/lib --enable-threadsafe --with-pthread=/usr/include/ --enable-hl --enable-shared --enable-unsupported \
    && make -j12 \
    && make install

RUN cd tmp \
    && wget https://github.com/microsoft/SEAL/archive/v3.4.4.tar.gz \
    && tar xvfz v3.4.4.tar.gz && cd SEAL-3.4.4/native/src \
    && cmake . \
    && make -j12 \
    && make install

ENV PATH $PATH:/usr/local/hdf5/bin
ENV LIBRARY_PATH $LIBRARY_PATH:/usr/local/hdf5/lib
ENV LD_LIBRARY_PATH /usr/lib:$LD_LIBRARY_PATH:/usr/local/hdf5/lib

WORKDIR /root/PP-CNN
COPY . /root/PP-CNN

RUN mkdir build \
    && cd build \
    && cmake .. \
    && make -j12

RUN mkdir -p datasets/mnist && cd datasets/mnist \
    && wget http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz \
    && wget http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz \
    && wget http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz \
    && wget http://yann.lecun.com/exdb/mnist/t10k-labels-idx1-ubyte.gz \
    && gunzip *.gz
RUN mkdir -p datasets/cifar-10 && cd datasets/cifar-10 \
    && wget https://www.cs.toronto.edu/~kriz/cifar-10-binary.tar.gz \
    && tar zxvf cifar-10-binary.tar.gz \
    && mv cifar-10-batches-bin/* ./ && rmdir cifar-10-batches-bin

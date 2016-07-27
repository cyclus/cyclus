
FROM ubuntu:16.04

RUN echo "1"

RUN apt-get -y --force-yes update

RUN apt-get install -y --force-yes \
    cmake \
    make \
    libboost-all-dev \
    libxml2-dev \
    libxml++2.6-dev \
    libsqlite3-dev \
    libhdf5-serial-dev \
    libbz2-dev \
    coinor-libcbc-dev \
    coinor-libcoinutils-dev \
    coinor-libosi-dev \
    coinor-libclp-dev \
    coinor-libcgl-dev \
    libblas-dev \
    liblapack-dev \
    g++ \
    libgoogle-perftools-dev \
    git \
    python \
    python-tables \
    python-numpy \
    python-nose

# build cyclus deb
COPY . /cyclus 
WORKDIR cyclus
RUN mkdir -p deb
WORKDIR /cyclus/deb
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr -DCPACK_SYSTEM_NAME=ubuntu.16.04 ..
RUN cpack ..

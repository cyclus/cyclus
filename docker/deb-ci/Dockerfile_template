FROM ubuntu:CYCLUS_UBUNTU_MAJOR_VERSION.04

# Because we used specific Ubuntu version can't use cyclus-deps docker container
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
    python-nose \
    python-pandas \
    python-jinja2 \
    cython

COPY . /cyclus 
WORKDIR cyclus
RUN mkdir -p deb
WORKDIR /cyclus/deb
RUN cmake -DCPACK_PACKAGING_INSTALL_PREFIX=/usr/local -DCMAKE_INSTALL_PREFIX=/usr/local -DCPACK_HASH_NAME=CYCLUS_REPO_HASH -DCPACK_SYSTEM_NAME=ubuntu.CYCLUS_UBUNTU_MAJOR_VERSION.04 ..

# Build the deb file in the docker container
RUN cpack -v ..

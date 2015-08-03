
FROM ubuntu

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
    python3 \
    python3-tables \
    python3-numpy \
    python3-nose

RUN rm /usr/bin/python
RUN ln -s /usr/bin/python3 /usr/bin/python


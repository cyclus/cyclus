FROM ubuntu:CYCAMORE_UBUNTU_MAJOR_VERSION.04

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
    cython \
    wget

RUN wget -r --no-parent -A 'cyclus_1.5.0_*_ubuntu.CYCAMORE_UBUNTU_MAJOR_VERSION.04.deb' http://dory.fuelcycle.org:4848
run mv dory.fuelcycle.org:4848/* ./; rm -rf dory.fuelcycle.org:4848
RUN dpkg -i cyclus_1.5.0_*_ubuntu.CYCAMORE_UBUNTU_MAJOR_VERSION.04.deb

COPY . /cycamore 
WORKDIR cycamore
RUN mkdir -p deb
WORKDIR /cycamore/deb
RUN cmake -DCPACK_PACKAGING_INSTALL_PREFIX=/usr/local -DCMAKE_INSTALL_PREFIX=/usr/local -DCPACK_HASH_NAME=CYCAMORE_REPO_HASH -DCPACK_SYSTEM_NAME=ubuntu.CYCAMORE_UBUNTU_MAJOR_VERSION.04 ..

# Build the deb file in the docker container
RUN cpack -v ..


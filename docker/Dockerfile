ARG pkg_mgr=apt
ARG make_cores=2

FROM ubuntu:22.04 as common-base

ENV TZ=America/Chicago
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt update --fix-missing

FROM common-base as apt-deps

RUN apt install -y \
        libssh-dev \
        g++ \
        gcc \
        cmake \
        make \
        libxml2-dev \
        libxml++2.6-dev \
        libblas-dev \
        liblapack-dev \
        pkg-config \
        coinor-libCbc-dev \
        coinor-libClp-dev \
        coinor-libcoinutils-dev \
        coinor-libOsi-dev \
        libboost-all-dev \
        libhdf5-dev \
        libsqlite3-dev \
        libpcre2-dev \
        gettext-base \
        xz-utils \
        python3-setuptools \
        python3-pytest \ 
        python3-tables \
        python3-pandas \
        python3-jinja2 \
        cython3 \
        libwebsockets-dev \
        python3-pprintpp \
    && apt clean -y all

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 10

FROM common-base as conda-deps

RUN apt install -y \
        wget \
        bzip2 \
        ca-certificates \
    && apt clean -y all

RUN echo 'export PATH=/opt/conda/bin:$PATH' > /etc/profile.d/conda.sh && \
    wget --quiet https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda.sh && \
    /bin/bash ~/miniconda.sh -b -p /opt/conda && \
    rm ~/miniconda.sh

ENV PATH /opt/conda/bin:$PATH

RUN conda config --add channels conda-forge
RUN conda update -n base -c defaults conda
RUN conda install -y mamba
RUN conda update -y --all && \
    mamba install -y \
               openssh \
               gxx_linux-64 \
               gcc_linux-64 \
               cmake \
               make \
               docker-pycreds \
               git \
               xo \
               python-json-logger \
               glib \
               libxml2 \
               libxmlpp \
               libblas \
               libcblas \
               liblapack \
               pkg-config \
               coincbc \
               boost-cpp \
               hdf5 \
               sqlite \
               pcre \
               gettext \
               bzip2 \
               xz \
               setuptools \
               pytest \
               pytables \
               pandas \
               jinja2 \
               "cython<3" \
               websockets \
               pprintpp \
               && \
    mamba install -y --force-reinstall libsqlite && \
    conda clean -y --all
ENV CC /opt/conda/bin/x86_64-conda_cos6-linux-gnu-gcc
ENV CXX /opt/conda/bin/x86_64-conda_cos6-linux-gnu-g++
ENV CPP /opt/conda/bin/x86_64-conda_cos6-linux-gnu-cpp
ENV PYTHONPATH "/root/.local/lib/python3.10/site-packages/"

FROM ${pkg_mgr}-deps as cyclus
ARG make_cores=2

# required for the nosetest
ENV PYTHONWARNINGS ignore
ENV PATH /root/.local/bin:$PATH

COPY . /cyclus
WORKDIR /cyclus

# Uncomment the following line to run cmake in verbose mode.
# This is sometimes useful for debugging.
#ENV VERBOSE=1

# You may add the option "--cmake-debug" to the following command
# for further CMake debugging.
RUN python install.py -j ${make_cores} --build-type=Release --core-version 999999.999999 

FROM cyclus as cyclus-test

RUN cyclus_unit_tests



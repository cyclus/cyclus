FROM ubuntu:22.04 as base

RUN apt-get update --fix-missing && apt-get install -y wget bzip2 ca-certificates \
    libglib2.0-0 libxext6 libsm6 libxrender1


RUN echo 'export PATH=/opt/conda/bin:$PATH' > /etc/profile.d/conda.sh && \
    wget --quiet https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda.sh && \
    /bin/bash ~/miniconda.sh -b -p /opt/conda && \
    rm ~/miniconda.sh

ENV PATH /root/.local/bin:/opt/conda/bin:$PATH


#
# apt packages
#
RUN apt-get update && \
    apt-get install -y openssh-client \
                       git \
                       vim nano && \
    apt-get clean

#
# conda packages
#
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
               cython \
               websockets \
               pprintpp \
               && \
    conda clean -y --all
ENV CC /opt/conda/bin/x86_64-conda_cos6-linux-gnu-gcc
ENV CXX /opt/conda/bin/x86_64-conda_cos6-linux-gnu-g++
ENV CPP /opt/conda/bin/x86_64-conda_cos6-linux-gnu-cpp
ENV PYTHONPATH "/root/.local/lib/python3.10/site-packages/"
# required for the nosetest
ENV PYTHONWARNINGS ignore
RUN mkdir -p /root/.local/lib/python3.10/site-packages/
#
# pip packages to overide conda
#

RUN pip install docker

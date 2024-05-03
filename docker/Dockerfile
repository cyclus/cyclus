ARG pkg_mgr=apt
ARG ubuntu_version=22.04

FROM ubuntu:${ubuntu_version} as common-base

ENV TZ=America/Chicago
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

FROM common-base as apt-deps

RUN apt update -y && apt install -y \
        g++ \
        gcc \
        cmake \
        make \
        libxml2-dev \
        libxml++2.6-dev \
        liblapack-dev \
        pkg-config \
        coinor-libcbc-dev \
        coinor-libclp-dev \
        coinor-libcoinutils-dev \
        coinor-libosi-dev \
        libboost-all-dev \
        libhdf5-dev \
        libsqlite3-dev \
        libpcre2-dev \
        python3-setuptools \
        python3-pytest \ 
        python3-tables \
        python3-pandas \
        python3-jinja2 \
        python3-pip \
        cython3 \
        libwebsockets-dev 

RUN apt install -y python3-pprintpp; exit 0
RUN apt clean -y all
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 10
RUN mkdir -p `python3 -m site --user-site`

FROM common-base as conda-deps

RUN apt update -y && apt install -y \
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
RUN conda install -y conda-libmamba-solver
RUN conda config --set solver libmamba
RUN conda install -y mamba
RUN conda uninstall -y conda-libmamba-solver
RUN conda config --set solver classic
RUN conda update -y --all && \
    mamba install -y \
               gxx_linux-64 \
               gcc_linux-64 \
               cmake \
               make \
               git \
               glib \
               libxml2 \
               libxmlpp-4.0 \
               liblapack \
               pkg-config \
               coincbc \
               libboost-devel \
               hdf5 \
               sqlite \
               pcre \
               setuptools \
               pytest \
               pytables \
               pandas \
               jinja2 \
               cython \
               websockets \
               pprintpp \
               pip \
               && \
    mamba install -y --force-reinstall libsqlite && \
    conda clean -y --all
RUN mkdir -p `python3 -m site --user-site`
ENV CC /opt/conda/bin/x86_64-conda_cos6-linux-gnu-gcc
ENV CXX /opt/conda/bin/x86_64-conda_cos6-linux-gnu-g++
ENV CPP /opt/conda/bin/x86_64-conda_cos6-linux-gnu-cpp

FROM apt-deps as apt-coverage-deps
RUN apt install -y lcov curl
ENV GCOV /usr/bin/gcov

FROM conda-deps as conda-coverage-deps
RUN mamba install -y lcov curl
ENV GCOV /opt/conda/bin/x86_64-conda-linux-gnu-gcov

FROM ${pkg_mgr}-coverage-deps as cyclus-coverage
ARG make_cores=2

COPY . /cyclus
WORKDIR /cyclus

RUN python3 install.py -j ${make_cores} --allow-milps --code-coverage

ENV PATH /root/.local/bin:$PATH
ENV LD_LIBRARY_PATH /root/.local/lib:/root/.local/lib/cyclus
RUN lcov -c -d /cyclus -o initial_coverage.info --gcov-tool ${GCOV} --initial --no-external && \
    cyclus_unit_tests && \
    cd tests && python3 -m pytest && cd .. && \
    lcov -c -d /cyclus -o test_coverage.info --gcov-tool ${GCOV} --no-external && \
    lcov --add-tracefile initial_coverage.info --add-tracefile test_coverage.info -o total_coverage.info && \
    mkdir -p html && genhtml total_coverage.info --output-directory html

FROM scratch as coverage-report
COPY --from=cyclus-coverage /cyclus /

FROM ${pkg_mgr}-deps as cyclus
ARG make_cores=2

COPY . /cyclus
WORKDIR /cyclus

# Uncomment the following line to run cmake in verbose mode.
# This is sometimes useful for debugging.
#ENV VERBOSE=1

# You may add the option "--cmake-debug" to the following command
# for further CMake debugging.
RUN python3 install.py -j ${make_cores} --allow-milps
ENV PATH /root/.local/bin:$PATH

FROM cyclus as deb-generation
WORKDIR /cyclus/build
RUN make package

FROM scratch as deb-package
COPY --from=deb-generation /cyclus/build/cyclus*.deb /

FROM cyclus as cyclus-test

RUN cyclus_unit_tests

FROM cyclus-test as cyclus-pytest

RUN cd tests && python3 -m pytest

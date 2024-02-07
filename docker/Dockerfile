ARG pkg_mgr=apt
ARG ubuntu_version=22.04

FROM ubuntu:${ubuntu_version} as common-base

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
        coinor-libcbc-dev \
        coinor-libclp-dev \
        coinor-libcoinutils-dev \
        coinor-libosi-dev \
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
        python3-pip \
        cython3 \
        libwebsockets-dev 

RUN apt install -y python3-pprintpp; exit 0
RUN apt clean -y all

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 10
RUN mkdir -p `python -m site --user-site`

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
RUN conda install -y conda-libmamba-solver
RUN conda config --set solver libmamba
RUN conda install -y mamba
RUN conda uninstall -y conda-libmamba-solver
RUN conda config --set solver classic
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
               libxmlpp-4.0 \
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
               pip \
               && \
    mamba install -y --force-reinstall libsqlite && \
    conda clean -y --all
RUN mkdir -p `python -m site --user-site`
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
RUN python install.py -j 2 --build-type=Release --core-version 999999.999999 --allow-milps --code_coverage
ENV PATH /root/.local/bin:$PATH
ENV LD_LIBRARY_PATH /root/.local/lib:/root/.local/lib/cyclus
RUN lcov -c --rc lcov_branch_coverage=1 -d build/src -o initial_coverage.info --gcov-tool ${GCOV} --initial && \
    cyclus_unit_tests && \
    cd tests && \
    python -m pytest --ignore test_main.py && \
    cd .. && \
    lcov -c --rc lcov_branch_coverage=1 -d build/src -o test_coverage.info --gcov-tool ${GCOV} && \
    lcov --add-tracefile initial_coverage.info --add-tracefile test_coverage.info --rc lcov_branch_coverage=1 -o total_coverage.info && \
    lcov --remove total_coverage.info -o src_coverage.info --rc lcov_branch_coverage=1 "/usr/**" "/opt/**" && \
    mkdir -p coverage/html && \
    genhtml src_coverage.info --output-directory coverage/html --branch-coverage
RUN --mount=type=bind,target=. \
    --mount=type=secret,id=GITHUB_TOKEN \
    ls /run/secrets
RUN --mount=type=bind,target=. \
    --mount=type=secret,id=GITHUB_TOKEN \
    curl -L https://coveralls.io/coveralls-linux.tar.gz | tar -xz -C /usr/local/bin && \
    COVERALLS_REPO_TOKEN=$(cat /run/secrets/GITHUB_TOKEN) coveralls report src_coverage.info --format=lcov --compare-ref=main -p --job-flag ${ubuntu-version}-${pkg_mgr}

FROM ${pkg_mgr}-deps as cyclus
ARG make_cores=2

COPY . /cyclus
WORKDIR /cyclus

# Uncomment the following line to run cmake in verbose mode.
# This is sometimes useful for debugging.
#ENV VERBOSE=1

# You may add the option "--cmake-debug" to the following command
# for further CMake debugging.
RUN python install.py -j ${make_cores} --build-type=Release --core-version 999999.999999 --allow-milps
ENV PATH /root/.local/bin:$PATH
ENV LD_LIBRARY_PATH /root/.local/lib:/root/.local/lib/cyclus

FROM cyclus as cyclus-test

RUN cyclus_unit_tests

FROM cyclus-test as cyclus-pytest

RUN cd tests && python -m pytest --ignore test_main.py


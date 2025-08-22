ARG pkg_mgr=apt
ARG ubuntu_version=24.04

FROM ubuntu:${ubuntu_version} AS common-base

ENV TZ=America/Chicago
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

FROM common-base AS apt-deps

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
        libwebsockets-dev \
        doxygen

RUN apt install -y python3-pprintpp; exit 0
RUN apt clean -y all

RUN mkdir -p `python3 -m site --user-site`

FROM common-base AS conda-deps

RUN apt update -y && apt install -y \
        wget \
        bzip2 \
        ca-certificates \
    && apt clean -y all

RUN echo 'export PATH=/opt/conda/bin:$PATH' > /etc/profile.d/conda.sh && \
    wget --quiet "https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-$(uname)-$(uname -m).sh" -O ~/miniforge.sh && \
    /bin/bash ~/miniforge.sh -b -p /opt/conda && \
    rm ~/miniforge.sh

ENV PATH=/opt/conda/bin:$PATH
RUN mamba create -n cyclus
SHELL ["mamba", "run", "-n", "cyclus", "/bin/bash", "-c"]
RUN mamba install -y "python<3.13" --no-pin && \
    mamba update -y --all && \
    mamba install -y \
               compilers \
               cmake = 4.0.3 \
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
               "cython<3.1.0" \
               websockets \
               pprintpp \
               pip \
               doxygen \
               && \
    mamba clean -y --all
RUN mkdir -p $(python3 -m site --user-site)

FROM apt-deps AS apt-coverage-deps
RUN apt install -y lcov curl
ENV GCOV=/usr/bin/gcov

FROM conda-deps AS conda-coverage-deps
RUN mamba install -y lcov curl
ENV GCOV=/opt/conda/bin/x86_64-conda-linux-gnu-gcov

FROM ${pkg_mgr}-coverage-deps AS cyclus-coverage
ARG make_cores=2
ARG build_flags="--parallel"
COPY . /cyclus
WORKDIR /cyclus

RUN python3 install.py -j ${make_cores} --allow-milps --code-coverage ${build_flags}
ENV PATH=/root/.local/bin:$PATH
ENV LCOV_ARGS="--gcov-tool=${GCOV} --no-external"
RUN lcov -c -d /cyclus -o initial_coverage.info --initial ${LCOV_ARGS} && \
    cyclus_unit_tests && \
    cd tests && python3 -m pytest && cd .. && \
    lcov -c -d /cyclus -o test_coverage.info ${LCOV_ARGS} && \
    lcov --add-tracefile initial_coverage.info --add-tracefile test_coverage.info -o temp_coverage.info && \
    lcov --remove temp_coverage.info -o total_coverage.info '/cyclus/build/_deps/**' && \
    mkdir -p html && genhtml total_coverage.info --output-directory html

FROM scratch AS coverage-report
COPY --from=cyclus-coverage /cyclus /

FROM ${pkg_mgr}-deps AS cyclus
ARG make_cores=2
ARG build_flags="--parallel"
COPY . /cyclus
WORKDIR /cyclus

# Uncomment the following line to run cmake in verbose mode.
# This is sometimes useful for debugging.
#ENV VERBOSE=1

# You may add the option "--cmake-debug" to the following command
# for further CMake debugging.
RUN python3 install.py -j ${make_cores} --allow-milps ${build_flags}
ENV PATH=/root/.local/bin:$PATH

FROM cyclus AS deb-generation
WORKDIR /cyclus/build
RUN make package

FROM scratch AS deb-package
COPY --from=deb-generation /cyclus/build/cyclus*.deb /

FROM cyclus AS cyclus-test

RUN cyclus_unit_tests

FROM cyclus-test AS cyclus-pytest

RUN cd tests && python3 -m pytest

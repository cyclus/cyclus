ARG rocky_version=9
FROM rockylinux:${rocky_version} as common-base

ENV TZ=America/Chicago
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN dnf update -y

FROM common-base as dnf-deps
RUN alternatives --install /usr/bin/python python /bin/python3 10
RUN dnf install -y 'dnf-command(config-manager)' && \
    dnf config-manager --set-enabled crb && \
    dnf install -y epel-release

RUN dnf install -y \
        wget \
        which \
        git \
        gcc \
        gcc-c++ \
        make \
        cmake \
        hdf5-devel \
        libxml2-devel \
        boost-devel \
        liblas-devel \
        lapack-devel \
        sqlite-devel \
        pcre2-devel \
        gettext \
        xz \
        python3-devel \
        python3-setuptools \
        python3-pip \
        python3-pytest \
        python3-jinja2 \
        python3-tables \
    && dnf clean all

RUN mkdir -p $(python -m site --user-site) && python -m pip install pandas cython

FROM dnf-deps as libxmlpp
ENV PATH /usr/local/bin:$PATH
RUN dnf install -y m4 doxygen perl-open perl-XML-Parser diffutils && \
    python -m pip install meson ninja && \
    wget https://github.com/libxmlplusplus/libxmlplusplus/releases/download/4.0.3/libxml++-4.0.3.tar.xz && \
    tar xf libxml++-4.0.3.tar.xz && \
    cd libxml++-4.0.3 && \
    meson setup --prefix /usr --libdir lib64 \
        --buildtype=release \
        -Dbuild-documentation=false\
        -Dbuild-manual=false \
        -Dmm-common:use-network=true \
        -Dglibmm-2.68:build-documentation=false \
        -Dglibmm-2.68:build-examples=false \
        -Dsigc++-3.0:build-documentation=false \
        -Dsigc++-3.0:build-examples=false \
        -Dsigc++-3.0:build-manual=false \
        build_dir . && \
    cd build_dir && \
    ninja && \
    ninja install


FROM libxmlpp as cyclus
ARG make_cores=2

COPY . /cyclus
WORKDIR /cyclus

# Uncomment the following line to run cmake in verbose mode.
# This is sometimes useful for debugging.
#ENV VERBOSE=1

# You may add the option "--cmake-debug" to the following command
# for further CMake debugging.
RUN python install.py -j ${make_cores} --build-type=Release --core-version 999999.999999
ENV PATH /root/.local/bin:$PATH
ENV LD_LIBRARY_PATH /root/.local/lib:/root/.local/lib/cyclus

FROM cyclus as cyclus-test

RUN cyclus_unit_tests

FROM cyclus-test as cyclus-pytest

RUN cd tests && python -m pytest


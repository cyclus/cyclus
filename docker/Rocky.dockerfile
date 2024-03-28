ARG rocky_version=9
FROM rockylinux:${rocky_version} as common-base

ENV TZ=America/Chicago
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN dnf update -y && \
    dnf install -y 'dnf-command(config-manager)' && \
    dnf install -y python3.11 epel-release

FROM common-base as rocky-8-config
RUN alternatives --set python /usr/bin/python3.11 && \
    alternatives --set python3 /usr/bin/python3.11 && \
    dnf config-manager --set-enabled powertools

FROM common-base as rocky-9-config
RUN alternatives --install /usr/bin/python python /bin/python3.11 10 && \
    alternatives --install /usr/bin/python3 python3 /bin/python3.11 10 && \
    dnf config-manager --set-enabled crb

FROM rocky-${rocky_version}-config as dnf-deps
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
        blas-devel \
        lapack-devel \
        sqlite-devel \
        gettext \
        xz \
        python3.11-devel \
        python3.11-setuptools \
        python3.11-pip \
        python3.11-pytest \
    && dnf clean all

RUN mkdir -p $(python -m site --user-site) && python -m pip install pandas tables cython jinja2

FROM dnf-deps as libxmlpp
RUN dnf install -y m4 doxygen perl-open perl-XML-Parser diffutils pcre-cpp pcre-devel  && \
    python -m pip install meson ninja packaging && \
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
RUN python install.py -j ${make_cores} --build-type=Release --parallel -D Python3_EXECUTABLE=/usr/bin/python3.11
ENV PATH /root/.local/bin:$PATH
ENV LD_LIBRARY_PATH /root/.local/lib:/root/.local/lib/cyclus

FROM cyclus as cyclus-test

RUN cyclus_unit_tests

FROM cyclus-test as cyclus-pytest

RUN cd tests && python -m pytest


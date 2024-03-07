FROM rockylinux:9 as common-base

ENV TZ=America/Chicago
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN yum update -y

FROM common-base as yum-deps
RUN alternatives --install /usr/bin/python python /bin/python3 10
RUN yum install -y 'dnf-command(config-manager)' &&  yum config-manager --set-enabled crb && yum install -y epel-release

RUN yum install -y \
        wget \
        which \
        git \
        gcc \
        gcc-c++ \
        make \
        cmake \
        hdf5-devel \
        libxml2-devel \
        libxml++-devel \
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
    && yum clean all

RUN mkdir -p $(python -m site --user-site) && python -m pip install pandas cython

FROM yum-deps as cyclus
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

FROM cyclus as deb-generation
WORKDIR /cyclus/build
RUN make package

FROM scratch as deb-package
COPY --from=deb-generation /cyclus/build/cyclus*.deb /

FROM cyclus as cyclus-test

RUN cyclus_unit_tests

FROM cyclus-test as cyclus-pytest

RUN cd tests && python -m pytest

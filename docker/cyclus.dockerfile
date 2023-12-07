ARG pkg_mgr=apt
ARG ubuntu_version=22.04
FROM ghcr.io/cyclus/cyclus_${ubuntu_version}_${pkg_mgr}/${pkg_mgr}-deps as cyclus
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

FROM cyclus

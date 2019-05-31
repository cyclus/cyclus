FROM cyclus/cyclus-deps

COPY . /cyclus
WORKDIR /cyclus

# Uncomment the following line to run cmake in verbose mode.
# This is sometimes useful for debugging.
#ENV VERBOSE=1

# You may add the option "--cmake-debug" to the following command
# for further CMake debugging.
RUN python install.py -j 2 --build-type=Release --core-version 999999.999999 \
    -DBLAS_LIBRARIES="/opt/conda/lib/libblas.so" \
    -DLAPACK_LIBRARIES="/opt/conda/lib/liblapack.so"


FROM cyclus/cyclus:latest

COPY . /cycamore
WORKDIR /cycamore
ENV PATH="/root/.local/bin:${PATH}"
RUN python install.py -j 2 --build-type=Release \
    -DBLAS_LIBRARIES="/opt/conda/lib/libblas.so" \
    -DLAPACK_LIBRARIES="/opt/conda/lib/liblapack.so"

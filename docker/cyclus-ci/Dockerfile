FROM cyclus/cyclus-deps

COPY . /cyclus
WORKDIR /cyclus
RUN conda update --all && conda clean --all
RUN python install.py -j2 --build-type=Release --core-version 999999.999999 \
    -DBLAS_LIBRARIES="openblas" -DLAPACK_LIBRARIES="openblas"


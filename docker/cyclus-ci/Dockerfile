FROM cyclus/cyclus-deps

COPY . /cyclus
WORKDIR /cyclus
#RUN conda update -y --all && conda clean -y --all
RUN python install.py -j2 --build-type=Release --core-version 999999.999999 \
    -DBLAS_LIBRARIES="/opt/conda/lib/libopenblas.so" \
    -DLAPACK_LIBRARIES="/opt/conda/lib/liblapack.so" \
    --deps-root=/opt/conda


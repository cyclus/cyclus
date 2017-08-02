FROM cyclus/cyclus-deps

COPY . /cyclus
WORKDIR /cyclus
ENV VERBOSE=1
#RUN conda update -y --all && conda clean -y --all
RUN python install.py -j2 --build-type=Release --core-version 999999.999999 \
    -DBLAS_LIBRARIES="/opt/conda/lib/libblas.so" \
    -DLAPACK_LINKER_FLAGS="/opt/conda/lib/libgfortran.so" \
    -DLAPACK_LIBRARIES="/opt/conda/lib/liblapack.so" \
    --deps-root=/opt/conda


#    --cmake-debug \



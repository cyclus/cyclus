FROM condaforge/linux-anvil-comp7
SHELL ["/bin/bash", "-c"]
RUN source /opt/conda/etc/profile.d/conda.sh && \
    conda install -y gxx_linux-64 gcc_linux-64 cmake make docker-pycreds git xo python-json-logger \
                     python=3.6 glibmm glib=2.56 libxml2 libxmlpp libblas \
                     libcblas liblapack pkg-config coincbc=2.9 boost-cpp hdf5 \
                     sqlite pcre gettext bzip2 xz setuptools nose pytables \
                     pandas jinja2 cython websockets pprintpp &&\
    conda clean -y --all

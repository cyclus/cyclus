FROM debian:9 

RUN apt-get update --fix-missing && apt-get install -y wget bzip2 ca-certificates \
    libglib2.0-0 libxext6 libsm6 libxrender1

RUN echo 'export PATH=/opt/conda/bin:$PATH' > /etc/profile.d/conda.sh && \
    wget --quiet https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda.sh && \
    /bin/bash ~/miniconda.sh -b -p /opt/conda && \
    rm ~/miniconda.sh

RUN apt-get install -y curl grep sed dpkg && \
    TINI_VERSION=`curl https://github.com/krallin/tini/releases/latest | grep -o "/v.*\"" | sed 's:^..\(.*\).$:\1:'` && \
    curl -L "https://github.com/krallin/tini/releases/download/v${TINI_VERSION}/tini_${TINI_VERSION}.deb" > tini.deb && \
    dpkg -i tini.deb && \
    rm tini.deb && \
    apt-get clean

ENV PATH /root/.local/bin:/opt/conda/bin:$PATH

ENTRYPOINT [ "/usr/bin/tini", "--" ]
CMD [ "/bin/bash" ]

#
# apt packages
#
RUN apt-get update && \
    apt-get install -y vim nano && \
    apt-get clean

#
# conda packages
#
RUN conda config --add channels conda-forge
RUN conda update -y --all && \
    conda install -y openssh gxx_linux-64 gcc_linux-64 cmake make docker-pycreds git xo python-json-logger \
	                     python=3.6 glibmm glib=2.56 libxml2 libxmlpp libblas \
	                     libcblas liblapack pkg-config coincbc=2.9 boost-cpp hdf5 \
	                     sqlite pcre gettext bzip2 xz setuptools nose pytables \
	                     pandas jinja2 cython==0.26 websockets pprintpp &&\
    conda clean -y --all

ENV CC /opt/conda/bin/x86_64-conda_cos6-linux-gnu-gcc
ENV CXX /opt/conda/bin/x86_64-conda_cos6-linux-gnu-g++
ENV CPP /opt/conda/bin/x86_64-conda_cos6-linux-gnu-cpp


#
# pip packages to overide conda
#
RUN pip install docker

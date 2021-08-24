FROM debian:9

RUN apt-get update --fix-missing && apt-get install -y wget bzip2 ca-certificates \
    libglib2.0-0 libxext6 libsm6 libxrender1

RUN echo 'export PATH=/opt/conda/bin:$PATH' > /etc/profile.d/conda.sh && \
    wget --quiet https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda.sh && \
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
    apt-get install -y openssh-client \
                       git \
                       vim nano && \
    apt-get clean

#
# conda packages
#
RUN conda config --add channels conda-forge
RUN conda update -n base -c defaults conda
RUN conda update -y --all && \
    conda install -y \
                _libgcc_mutex=0.1=main \
                _openmp_mutex=4.5=1_gnu \
                _sysroot_linux-64_curr_repodata_hack=3=haa98f57_10 \
                binutils_impl_linux-64=2.35.1=h27ae35d_9 \
                binutils_linux-64=2.35.1=h454624a_30 \
                blas=1.1=openblas \
                boost-cpp=1.70.0=h7b93d67_3 \
                bzip2=1.0.8=h7f98852_4 \
                ca-certificates=2021.5.25=h06a4308_1 \
                certifi=2021.5.30=py36h06a4308_0 \
                cmake=3.19.6=h973ab73_0 \
                make \
                nose \
                coincbc=2.9.9=blas_openblash0f40ac4_1002 \
                cython=0.28.5 \
                expat=2.4.1=h2531618_2 \
                gcc_impl_linux-64=9.3.0=h6df7d76_17 \
                gcc_linux-64=9.3.0=h1ee779e_30 \
                gettext=0.19.8.1=h0b5b191_1005 \
                glib=2.68.3=h9c3ff4c_0 \
                glib-tools=2.68.3=h9c3ff4c_0 \
                glibmm=2.58.1=h778ead0_1 \
                gxx_impl_linux-64=9.3.0=hbdd7822_17 \
                gxx_linux-64=9.3.0=h7e70986_30 \
                hdf5=1.10.6=hb1b8bf9_0 \
                icu=67.1=he1b5a44_0 \
                jinja2=3.0.0=pyhd3eb1b0_0 \
                kernel-headers_linux-64=3.10.0=h57e8cba_10 \
                krb5=1.18.2=h173b8e3_0 \
                ld_impl_linux-64=2.35.1=h7274673_9 \
                libblas=3.8.0=17_openblas \
                libcblas=3.8.0=17_openblas \
                libcurl=7.71.1=h20c2e04_1 \
                libedit=3.1.20210216=h27cfd23_1 \
                libffi=3.3=he6710b0_2 \
                libgcc-devel_linux-64=9.3.0=hb95220a_17 \
                libgcc-ng=9.3.0=h5101ec6_17 \
                libgfortran-ng=7.5.0=h14aa051_19 \
                libgfortran4=7.5.0=h14aa051_19 \
                libglib=2.68.3=h3e27bee_0 \
                libgomp=9.3.0=h5101ec6_17 \
                libiconv=1.16=h516909a_0 \
                liblapack=3.8.0=17_openblas \
                libopenblas=0.3.10=h5a2b251_0 \
                libsigcpp=2.10.3=ha770c72_0 \
                libssh2=1.9.0=h1ba5d50_1 \
                libstdcxx-devel_linux-64=9.3.0=hf0c5c8d_17 \
                libstdcxx-ng=9.3.0=hd4cf53a_17 \
                libuv=1.40.0=h7b6447c_0 \
                libxml2=2.9.10=h68273f3_2 \
                libxmlpp=2.40.1=h0bd56ce_1004 \
                lz4-c=1.9.3=h9c3ff4c_0 \
                markupsafe=2.0.1=py36h27cfd23_0 \
                ncurses=6.2=he6710b0_1 \
                numpy=1.19.5=py36h2aa4a07_1 \
                openblas=0.3.3=h9ac9557_1001 \
                openssl=1.1.1k=h27cfd23_0 \
                pandas=1.1.5=py36ha9443f7_0 \
                pcre=8.45=h9c3ff4c_0 \
                pip=21.1.2=py36h06a4308_0 \
                pkg-config=0.29.2=h1bed415_8 \
                python=3.6.13=h12debd9_1 \
                python-dateutil=2.8.1=pyhd3eb1b0_0 \
                python_abi=3.6=1_cp36m \
                pytz=2021.1=pyhd3eb1b0_0 \
                readline=8.1=h27cfd23_0 \
                rhash=1.4.1=h3c74f83_1 \
                setuptools=52.0.0=py36h06a4308_0 \
                sigcpp-2.0=2.10.3=h58526e2_0 \
                six=1.16.0=pyhd3eb1b0_0 \
                sqlite=3.35.4=hdfb4753_0 \
                sysroot_linux-64=2.17=h57e8cba_10 \
                tk=8.6.10=hbc83047_0 \
                wheel=0.36.2=pyhd3eb1b0_0 \
                xz=5.2.5=h7b6447c_0 \
                zlib=1.2.11=h7b6447c_3 \
                zstd=1.4.9=ha95c52a_0 \
    && \
    conda clean -y --all
ENV CC /opt/conda/bin/x86_64-conda_cos6-linux-gnu-gcc
ENV CXX /opt/conda/bin/x86_64-conda_cos6-linux-gnu-g++
ENV CPP /opt/conda/bin/x86_64-conda_cos6-linux-gnu-cpp
ENV PYTHONPATH "/home/conda/.local/lib/python3.7/site-packages/:/root/.local/lib/python3.7/site-packages/"
# required for the nosetest
ENV PYTHONWARNINGS ignore
RUN mkdir -p /root/.local/lib/python3.7/site-packages/
#
# pip packages to overide conda
#
RUN pip install docker

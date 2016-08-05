FROM cyclus/cyclus-deps

COPY . /cyclus
WORKDIR /cyclus
RUN python install.py -j2 --build_type=Release --prefix=/usr/local  --clean-build

FROM cyclus/cyclus-deps

COPY . /cyclus
WORKDIR /cyclus
RUN which python
RUN python install.py -j2 --build-type=Release --core-version 999999.999999

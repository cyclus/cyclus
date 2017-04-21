FROM cyclus/cyclus-deps

COPY . /cyclus
WORKDIR /cyclus
ENV PATH="/root/.local/bin:${PATH}"
RUN python install.py -j2 --build-type=Release --core-version 999999.999999

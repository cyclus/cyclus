FROM cyclus/cyclus-deps

COPY . /cyclus
WORKDIR /cyclus
RUN mkdir Release
WORKDIR /cyclus/Release
RUN pip install cython jinja2 numpy
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j2 && make install

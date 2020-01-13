
FROM cyclus/cyclus:stable

COPY . /cycamore
WORKDIR /cycamore
RUN mkdir Release
WORKDIR /cycamore/Release
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j2 && make install


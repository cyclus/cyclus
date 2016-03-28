
FROM cyclus/cyclus-deps

COPY . /cyclus
WORKDIR /cyclus
RUN mkdir Release
RUN mkdir deb
WORKDIR /cyclus/Release
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
-RUN make -j2 && make install
WORKDIR /cyclus/deb
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr ..
RUN cpack ..

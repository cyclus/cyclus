FROM cyclus/cyclus-deps

COPY . /cyclus
WORKDIR /cyclus
RUN python install.py -j2 --build-type=Release --core-version 999999.999999
#RUN echo "i am here $CIRCLE_BUILD_NUM" > iamhere.txt

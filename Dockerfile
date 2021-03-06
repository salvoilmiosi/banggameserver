FROM alpine:latest

RUN apk update && apk add jsoncpp
RUN apk add --no-cache --virtual .build_deps \
    g++ cmake ninja pkgconf linux-headers fmt-dev jsoncpp-dev yaml-cpp-dev

COPY . /usr/src/bang
WORKDIR /usr/src/bang/build

RUN cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. && cmake --build . && cmake --install .

WORKDIR /
RUN rm -rf /usr/src/bang && apk del .build_deps

EXPOSE 47654

CMD [ "/usr/local/bin/bangserver" ]
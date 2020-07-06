FROM alpine:latest
LABEL maintainer="Support<gijsbrecht.dewaegeneer@telenet.be>"

RUN apk update && apk add --virtual build-dependencies build-base clang cmake libusb-dev bash make


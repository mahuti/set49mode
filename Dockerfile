FROM alpine:latest
LABEL maintainer="---"

RUN apk update && apk add --virtual build-dependencies build-base clang cmake libusb-dev bash make


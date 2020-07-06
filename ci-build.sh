#!/usr/bin/env bash
mkdir -p builds/ci-build
cd builds/ci-build
CC=clang CXX=clang++ cmake ../..
make

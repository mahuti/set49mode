#!/usr/bin/env bash
mkdir -p builds/tidy-build
cd builds/tidy-build
CC=clang CXX=clang++ cmake \
    -DCMAKE_CXX_CLANG_TIDY="clang-tidy;-warnings-as-errors=*;-header-filter=$(realpath ..)" \
    ../..
make -k

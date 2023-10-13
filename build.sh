#!/bin/bash

cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_MAKE_PROGRAM=ninja \
    -G Ninja \
    -S ./ \
    -B ../build \
&& cd ../build \
&& ninja 
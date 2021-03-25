#!/bin/sh

set -e

# Build the wasm filter
cd wasm-filter
docker run -v $PWD:/work -w /work bladedancer/wasmsdk:v2 /build_wasm.sh
#docker run -v $PWD:/work -w /work bladedancer/wasmsdk:istio-release-v1.8 /build_wasm.sh
cd ..

# Build the test server
cd demoserver
make
cd  ..
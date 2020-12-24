#!/bin/bash

#  Build Software
mkdir -p cpp/release
pushd cpp/release
make -j4
if [ ! "$?" = '0' ]; then
    echo 'error running make. Aborting'
    exit
fi
popd

# Run Route Finder Tests
./cpp/release/bin/route_finder_tests



#!/bin/bash

#  Cleanup
rm waypoints.csv; 

#  Build Software
mkdir -p cpp/release
pushd cpp/release
make  
popd

# Run Route Finder
./cpp/release/bin/route_finder -d ./bike_data.db -sector_id 1 -i 300 -pop 1000 -minw 6 -maxw 14


#!/bin/bash

#  Cleanup
rm waypoints.csv; 

#  Build Software
mkdir -p cpp/release
pushd cpp/release
make  
popd

# Run Route Finder
./cpp/release/bin/route_finder -d ./bike_data.db -sector_id 1 -i 1000 -pop 3000 -maxw 14 -minw 14


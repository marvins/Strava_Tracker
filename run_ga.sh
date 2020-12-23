#!/bin/bash

#  Cleanup
rm waypoints.csv; 

#  Build Software
mkdir -p cpp/release
pushd cpp/release
make  
popd

# Run Route Finder
#./cpp/release/bin/route_finder -d ./bike_data.db -sector_id 1 -i 200 -pop 1000 -minw 6 -maxw 14 -nt 12 -start_point 39.6242729 -104.868715 -end_point 39.6451229 -104.869339

# Run Full Dataset
./cpp/release/bin/route_finder -d ./bike_data.db -i 400 -pop 3000 -minw 50 -maxw 50 -nt 12 -start_point 39.5989743 -104.8609468 -end_point 39.75428108249532 -105.00085402872664



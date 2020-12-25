#!/bin/bash

#  Cleanup
rm waypoints.csv; 

#  Build Software
mkdir -p cpp/release
pushd cpp/release
make  
if [ ! "$?" = '0' ]; then
    echo 'error running make. Aborting'
    exit
fi
popd

MIN_WP=40
MAX_WP=40

# Run Route Finder
./cpp/release/bin/route_finder -d ./bike_data.db -sector_id 1 -i 2000 -pop 2000 -minw ${MIN_WP} -maxw ${MAX_WP} -nt 12 -start_point 39.6242729 -104.868715 -end_point 39.6451229 -104.869339

# Run Full Dataset
#./cpp/release/bin/route_finder -d ./bike_data.db -i 1000 -pop 1000 -minw ${MIN_WP} -maxw ${MAX_WP} -nt 20 -start_point 39.5989743 -104.8609468 -end_point 39.75428108249532 -105.00085402872664



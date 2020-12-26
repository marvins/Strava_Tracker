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

POP_SIZE=2000

MIN_WP=40
MAX_WP=40

MUT_RATE='0.8'
SRATE='0.3'
PRATE='0.1'

# Run Route Finder on Sector-1
#./cpp/release/bin/route_finder -d ./bike_data.db -sector_id 2 -i 2000 -pop ${POP_SIZE} -minw ${MIN_WP} -maxw ${MAX_WP} \
#                                -nt 12 -start_point 39.6242729 -104.868715 -end_point 39.6451229 -104.869339 \
#                                -m ${MUT_RATE} -p ${PRATE} -s ${SRATE} -seed_id 2

# Run Route Finder on Sector-2
#./cpp/release/bin/route_finder -d ./bike_data.db -sector_id 2 -i 2000 -pop ${POP_SIZE} -minw ${MIN_WP} -maxw ${MAX_WP} \
#                               -nt 12 -start_point 39.6451229 -104.869339 -end_point 39.658093 -104.8630262 \
#                               -m ${MUT_RATE} -p ${PRATE} -s ${SRATE} -seed_id 2

# Run Full Dataset
./cpp/release/bin/route_finder -d ./bike_data.db -i 1000 -pop ${POP_SIZE} -minw ${MIN_WP} -maxw ${MAX_WP} \
                               -nt 20 -start_point 39.5989743 -104.8609468 -end_point 39.75428108249532 -105.00085402872664 \
                               -m ${MUT_RATE} -p ${PRATE} -s ${SRATE} -seed_id 2


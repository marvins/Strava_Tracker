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

ITERATIONS=1000
POP_SIZE=2000

MIN_WP=20
MAX_WP=40

MUT_RATE='1'
SRATE='0.4'
PRATE='0.05'

SEED_ID='2'

# Run Route Finder on Sector-1
#./cpp/release/bin/route_finder -d ./bike_data.db -sector_id 1 -i ${ITERATIONS} -pop ${POP_SIZE} -minw ${MIN_WP} -maxw ${MAX_WP} \
#                                -nt 12 -m ${MUT_RATE} -p ${PRATE} -s ${SRATE} -seed_id ${SEED_ID}

# Run Route Finder on Sector-2
./cpp/release/bin/route_finder -d ./bike_data.db -sector_id 2 -i ${ITERATIONS} -pop ${POP_SIZE} -minw ${MIN_WP} -maxw ${MAX_WP} \
                               -nt 12 -m ${MUT_RATE} -p ${PRATE} -s ${SRATE} -seed_id ${SEED_ID}

# Run Full Dataset
#./cpp/release/bin/route_finder -d ./bike_data.db -i ${ITERATIONS} -pop ${POP_SIZE} -minw ${MIN_WP} -maxw ${MAX_WP} \
#                               -nt 20 -m ${MUT_RATE} -p ${PRATE} -s ${SRATE} -seed_id ${SEED_ID}


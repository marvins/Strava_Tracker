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
POP_SIZE=1000

MIN_WP=8
MAX_WP=30

MUT_RATE='1'
SRATE='0.3'
PRATE='0.05'

SEED_ID='2'

MAX_ERR='40'

GA_THREADS='2'

SECTOR='-1'

# Run Route Finder
./cpp/release/bin/route_finder -d ./bike_data.db -i ${ITERATIONS} -pop ${POP_SIZE} -minw ${MIN_WP} -maxw ${MAX_WP} \
                               -gt ${GA_THREADS} -m ${MUT_RATE} -p ${PRATE} -s ${SRATE} -seed_id ${SEED_ID} -err ${MAX_ERR} \
                               -sector_id ${SECTOR}

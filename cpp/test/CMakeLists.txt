#  Name:   CMakeLists.txt
#  Author: Marvin Smith
#  Date:   12/19/2020

add_executable( route_finder_tests
                route_finder_test.cpp
                TEST_Geometry.cpp )

include_directories( '../googletest/googletest/include' )

target_link_libraries( route_finder_tests
                       SQLite::SQLite3
                       ${SOCI_LIBRARY}
                       ${Boost_LIBRARIES}
                       ${GDAL_LIBRARIES}
                       gtest )

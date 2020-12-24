#  Name:   CMakeLists.txt
#  Author: Marvin Smith
#  Date:   12/19/2020

add_executable( route_finder_tests
                route_finder_test.cpp
                TEST_Accumulator.cpp
                TEST_GDAL_Utilities.cpp
                TEST_Geometry.cpp
                TEST_KML_Writer.cpp
                TEST_Point.cpp
                TEST_QuadTree.cpp
                TEST_Rect.cpp
                TEST_Thread_Pool.cpp
                TEST_WaypointList.cpp
                ../src/Accumulator.hpp
                ../src/Blocking_Queue.hpp
                ../src/DB_Point.hpp
                ../src/DB_Point.cpp
                ../src/DB_Utils.hpp
                ../src/DB_Utils.cpp
                ../src/GDAL_Utilities.hpp
                ../src/GDAL_Utilities.cpp
                ../src/Geometry.hpp
                ../src/KML_Writer.hpp
                ../src/KML_Writer.cpp
                ../src/Point.hpp
                ../src/Point.cpp
                ../src/QuadTree.hpp
                ../src/Rect.hpp
                ../src/Stats_Aggregator.hpp
                ../src/Stats_Aggregator.cpp
                ../src/Thread_Pool.hpp
                ../src/WaypointList.hpp
                ../src/WaypointList.cpp )

include_directories( '../googletest/googletest/include' )

target_link_libraries( route_finder_tests
                       SQLite::SQLite3
                       ${SOCI_LIBRARY}
                       ${Boost_LIBRARIES}
                       ${GDAL_LIBRARIES}
                       gtest )

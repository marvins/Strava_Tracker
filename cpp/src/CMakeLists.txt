#  Name:   CMakeLists.txt
#  Author: Marvin Smith
#  Date:   12/19/2020

add_executable( route_finder
                route_finder.cpp
                Accumulator.hpp
                Blocking_Queue.hpp
                Context.hpp
                DB_Point.hpp
                DB_Point.cpp
                DB_Utils.hpp
                DB_Utils.cpp
                Exit_Condition.hpp
                Exit_Condition.cpp
                GA_Config.hpp
                GA_Config.cpp
                GDAL_Utilities.hpp
                GDAL_Utilities.cpp
                Genetic_Algorithm.hpp
                Geometry.hpp
                KML_Writer.hpp
                KML_Writer.cpp
                Options.hpp
                Options.cpp
                Point.hpp
                Point.cpp
                QuadTree.hpp
                Rect.hpp
                Sector_Runner.hpp
                Sector_Runner.cpp
                Stats_Aggregator.hpp
                Stats_Aggregator.cpp
                Thread_Pool.hpp
                WaypointList.hpp
                WaypointList.cpp
                Write_Worker.hpp
                Write_Worker.cpp )

target_link_libraries( route_finder
                       SQLite::SQLite3
                       ${SOCI_LIBRARY}
                       ${Boost_LIBRARIES}
                       ${GDAL_LIBRARIES}
                       ${CMAKE_DL_LIBS} )

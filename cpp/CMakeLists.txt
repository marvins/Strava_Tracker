#  Name:   CMakeLists.txt
#  Author: Marvin Smith
#  Date:   12/19/2020

cmake_minimum_required ( VERSION 3.0 )
project( Route_Finder )

find_package( SQLite3 )
find_package( Boost COMPONENTS system log program_options REQUIRED )
find_package( GDAL REQUIRED )

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(googletest)
add_subdirectory(src)
add_subdirectory(test)
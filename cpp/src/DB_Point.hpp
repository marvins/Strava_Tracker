/**
 * @file    Point.hpp
 * @author  Marvin Smith
 * @date    12/19/2020
 */
#pragma once

// C++ Libraries
#include <string>
#include <tuple>
#include <vector>

/**
 * @class Point
 * @brief Point object from database and used for waypoint estimation.
 */
class DB_Point
{
    public:

        std::string To_String()const;

        // Database Fields
        size_t index;
        double latitude;
        double longitude;
        int    gz;
        double easting;
        double northing;
        std::string timestamp;
        std::string sectorId;
        std::string datasetId;

        // Temporary Fields Used For Computations
        double x_norm;
        double y_norm;

}; // End of DB_Point Class

/**
 * @brief Normalize the Point Range
 * @return [minX,minY,maxX,maxY]
 */
std::tuple<int,int,int,int> Normalize_Points( std::vector<DB_Point>& point_list,
                                              int                    min_x = -1,
                                              int                    min_y = -1 );
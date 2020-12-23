/**
 * @file    Context.hpp
 * @author  Marvin Smith
 * @date    12/22/2020
*/
#pragma once

// C++ Libraries
#include <vector>

// Project Libraries
#include "DB_Point.hpp"
#include "Geometry.hpp"

struct Context
{
    // Reference Point List
    std::vector<DB_Point> point_list;
    std::vector<Point<double>> geo_point_list;

    // Density Step Distance
    double density_step_distance;

}; // End of Context Class
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
    std::vector<Point> geo_point_list;

    Point start_point;
    Point end_point;

}; // End of Context Class
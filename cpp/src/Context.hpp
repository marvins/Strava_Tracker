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
#include "QuadTree.hpp"

struct Context
{
    // Reference Point List
    std::vector<DB_Point> point_list;
    std::vector<Point> geo_point_list;

    Point start_point;
    Point end_point;

    // Density Step Distance
    double density_step_distance { 25 };

    // Quad Tree
    QuadTree<QTNode> point_quad_tree { Rect() };

}; // End of Context Class
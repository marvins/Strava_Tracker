/**
 * @file    Geometry.hpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#pragma once

// C++ Libraries
#include <array>
#include <cmath>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// Boost Geometry
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>

// Project Libraries
#include "QuadTree.hpp"
#include "Point.hpp"

/**
 * @brief Compute the distance from the point to the line segment
 * @param p Test point
 * @param l1 First point on line segment
 * @param l2 Second point on line segment
 */
 template <typename TP, size_t Dims>
double Point_Line_Distance( const Point_<TP,Dims>& p, 
                            const Point_<TP,Dims>& l1, 
                            const Point_<TP,Dims>& l2 )
{
    // Build lines
    auto v_21 = l2 - l1;
    auto v_p2 = p - l2;
    auto v_p1 = p - l1;

    int64_t p_1 = std::round( v_21.x() * v_p2.x() + v_21.y() * v_p2.y());
    int64_t p_2 = std::round( v_21.x() * v_p1.x() + v_21.y() * v_p1.y());

    if( p_1 > 0 )
    {
        return Point_<TP,Dims>::Distance_L2( p, l2 );
    }
    if( p_2 < 0 )
    {
        return Point_<TP,Dims>::Distance_L2( p, l1 );
    }
    // If the 2 line segment points are the same, just do distance
    if( v_21.Mag() < 0.01 )
    {
        return Point_<TP,Dims>::Distance_L2( p, l1 );
    }
    
    return std::fabs( v_21.x()*v_p1.y() - v_p1.x()*v_21.y()) / v_21.Mag();
}


/**
 * @brief Find the best segment error and density
 * 
 * This method does 2 things
 * 1.  Finds the minimum segment distance for a given reference point against all line segments.
 *
 * 2.  Checks to see if the reference point is within the "step distance" of any line segment. 
 */
template <typename TP, size_t Dims>
double Find_Best_Segment_Error( const Point_<TP,Dims>&              ref_point,
                                const std::vector<Point_<TP,Dims>>& vertices,
                                std::map<int,int>&                  segment_histogram )
{
    double minSegmentDist = -1;
    int segmentId = -1;

    // Iterate over all vertices, finding the nearest distance to a segment, then do it for each distance to vertex itself
    for( size_t i=0; i<(vertices.size()-1); i++ )
    {
        // Compute the distance to this line segment
        auto dist = Point_Line_Distance( ref_point, vertices[i], vertices[i+1] );
        if( minSegmentDist < 0 || dist < minSegmentDist )
        {
            minSegmentDist = dist;
            segmentId = i;
        }
    }
    segment_histogram[segmentId]++;
    return minSegmentDist;
}

/**
 * @brief March along the line segment, looking for any regions where there are no points present. 
 *        This will help reduce the impact of switchbacks or other behavior.
*/
template <typename TP, size_t Dims>
double Get_Segment_Density( const std::vector<Point_<TP,Dims>>& vertices,
                            const QuadTree<QTNode>&             quad_tree,
                            double                              step_distance )
{
    double segment_pos = 0;
    double segment_length = 0;
    bool point_found = false;
    double ratio = 0;
    uint64_t total_steps = 0;
    uint64_t steps_with_points = 1;
    std::vector<std::shared_ptr<QTNode>> results;

    // For each vertex
    for( size_t i=0; i<(vertices.size()-1); i++ )
    {
        // Reset position info for vertex
        segment_pos = 0;
        segment_length = Point_<TP,Dims>::Distance_L2( vertices[i], 
                                                 vertices[i+1] );

        // March along segment, one "radius distance" at a time
        while( true )
        {
            // Compute the ratio
            ratio = segment_pos / segment_length;

            // If the ratio means we have gone past the segment, break free of the while loop
            if( ratio > 1 )
            {
                break;
            }
            total_steps++;

            // Perform interpolation
            auto test_seg_point = Point_<TP,Dims>::LERP( vertices[i],
                                                   vertices[i+1],
                                                   ratio );
            

            // Look for a single point within distance to this point
            point_found = false;
            results = quad_tree.Search( test_seg_point, step_distance );
            if( results.size() > 0 )
            {
                 point_found = true;
                 steps_with_points++;
            }

            // Update segment position
            segment_pos += step_distance;
        }
    }

    return ((double)total_steps / steps_with_points);
}
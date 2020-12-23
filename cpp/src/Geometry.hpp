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

BOOST_GEOMETRY_REGISTER_BOOST_TUPLE_CS(boost::geometry::cs::cartesian)

/**
 * @class Point
 * @brief Utility Class for Point Operations
*/
template <typename value_type = double>
class Point
{
    public:

        Point() = default;

        Point( double x, double y )
        {
            m_data[0] = x;
            m_data[1] = y;
        }

        /**
         * @brief Get X Value
         */
        double x() const
        {
            return m_data[0];
        }
        double& x() 
        {
            return m_data[0];
        }

        /**
         * @brief Get Y Value
         */
        double y() const
        {
            return m_data[1];
        }
        double& y() 
        {
            return m_data[1];
        }

        /**
         * @brief Add in-place operator
         */
        Point<value_type>& operator += ( const Point<value_type>& rhs )
        {
            m_data[0] += rhs.m_data[0];
            m_data[1] += rhs.m_data[1];
            return *this;
        }

        /**
         * @brief Subtract in-place operator
         */
        Point<value_type>& operator -= ( const Point<value_type>& rhs )
        {
            m_data[0] -= rhs.m_data[0];
            m_data[1] -= rhs.m_data[1];
            return *this;
        }

        /**
         * @brief Magnitude
         */
        double Magnitude() const
        {
            return sqrt( x() * x() + y() * y() );
        }

        /**
         * @brief Distance
         */
        static double Distance_L2( const Point<value_type>& p1,
                                   const Point<value_type>& p2 )
        {
            return sqrt( (p1.x() - p2.x())*(p1.x() - p2.x()) + (p1.y() - p2.y())*(p1.y() - p2.y()));
        }

        /**
         * @brief Perform Linear Interpolation
        */
        static Point<value_type> LERP( const Point<value_type>& pt1, 
                                       const Point<value_type>& pt2,
                                       double           ratio )
        {
            return (((1-ratio) * pt1) + (ratio * pt2));
        }

        /**
         * @brief Print to Log-Friendly String
         */
        std::string To_String() const
        {
            std::stringstream sout;
            sout << "Point: " << std::fixed << x() << ", " << y();
            return sout.str();
        }

        // Underlying Datatype
        std::array<double,2> m_data { 0, 0 };

}; // End of Point Class

template <typename TP>
Point<TP> operator + ( const Point<TP>& p1, const Point<TP>& p2 )
{
    Point<TP> output( p1.m_data[0] + p2.m_data[0],
                      p1.m_data[1] + p2.m_data[1] );
    return output;
}

template <typename TP>
Point<TP> operator - ( const Point<TP>& p1, const Point<TP>& p2 )
{
    Point<TP> output( p1.m_data[0] - p2.m_data[0],
                      p1.m_data[1] - p2.m_data[1] );
    return output;
}

/**
 * @brief Perform Multiplication with a Scalar
*/
template <typename TP, typename S>
Point<TP> operator * ( const Point<TP>& pt, S scalar )
{
    Point<TP> output( pt.m_data[0] * scalar,
                      pt.m_data[1] * scalar );
    return output;
}

/**
 * @brief Perform Multiplication with a Scalar
*/
template <typename TP, typename S>
Point<TP> operator * ( S scalar, const Point<TP>& pt )
{
    Point<TP> output( scalar * pt.m_data[0],
                      scalar * pt.m_data[1] );
    return output;
}


/**
 * @brief Compute the distance from the point to the line segment
 * @param p Test point
 * @param l1 First point on line segment
 * @param l2 Second point on line segment
 */
template <typename TP>
double Point_Line_Distance( const Point<TP>& p, const Point<TP>& l1, const Point<TP>& l2 )
{
    // Build lines
    auto v_21 = l2 - l1;
    auto v_p2 = p - l2;
    auto v_p1 = p - l1;

    int64_t p_1 = std::round( v_21.x() * v_p2.x() + v_21.y() * v_p2.y());
    int64_t p_2 = std::round( v_21.x() * v_p1.x() + v_21.y() * v_p1.y());

    if( p_1 > 0 )
    {
        return Point<TP>::Distance_L2( p, l2 );
    }
    if( p_2 < 0 )
    {
        return Point<TP>::Distance_L2( p, l1 );
    }
    // If the 2 line segment points are the same, just do distance
    if( v_21.Magnitude() < 0.01 )
    {
        return Point<TP>::Distance_L2( p, l1 );
    }
    
    return std::fabs( v_21.x()*v_p1.y() - v_p1.x()*v_21.y()) / v_21.Magnitude();
}


/**
 * @brief Find the best segment error
 */
template <typename TP>
double Find_Best_Segment_Error( const Point<TP>&              ref_point,
                                const std::vector<Point<TP>>& vertices,
                                std::map<int,int>&            segment_histogram )
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
template <typename TP>
double Get_Segment_Density( const std::vector<Point<TP>>& vertices,
                            const std::vector<Point<TP>>& points,
                            double                        step_distance )
{
    double segment_pos = 0;
    double segment_length = 0;
    bool point_found = false;
    double ratio = 0;
    uint64_t total_steps = 0;
    uint64_t steps_with_points = 0;

    // For each vertex
    for( size_t i=0; i<(vertices.size()-1); i++ )
    {
        // Reset position info for vertex
        segment_pos = 0;
        segment_length = Point<TP>::Distance_L2( vertices[i], 
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
            auto test_seg_point = Point<TP>::LERP( vertices[i],
                                                   vertices[i+1],
                                                   ratio );
            

            // Look for a single point within distance to this point
            point_found = false;
            for( const auto& ref_point : points )
            {
                if( Point<TP>::Distance_L2( test_seg_point, ref_point ) < step_distance )
                {
                    point_found = true;
                    steps_with_points++;
                    break;
                }
            }

            // Update segment position
            segment_pos += step_distance;
        }
    }

    return ((double)total_steps / steps_with_points);
}
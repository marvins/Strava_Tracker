/**
 * @file    Geometry.hpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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
                                const std::vector<Point<TP>>& vertices )
{
    double minSegmentDist = -1;

    // Iterate over all vertices, finding the nearest distance to a segment, then do it for each distance to vertex itself
    for( size_t i=0; i<(vertices.size()-1); i++ )
    {
        // Compute the distance to this line segment
        auto dist = Point_Line_Distance( ref_point, vertices[i], vertices[i+1] );
        if( minSegmentDist < 0 || dist < minSegmentDist )
        {
            minSegmentDist = dist;
        }
    }
    return minSegmentDist;
}                                
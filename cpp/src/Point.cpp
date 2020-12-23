/**
 * @file    Point.cpp
 * @author  Marvin Smith
 * @date    12/23/2020
*/
#include "Point.hpp"

/****************************************/
/*          Convert to Point            */
/****************************************/
Point ToPoint2D( double x, double y )
{
    auto p = Point();
    p.m_data[0] = x;
    p.m_data[1] = y;
    return p;
}
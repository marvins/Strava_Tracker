/**
 * @file    TEST_Geometry.hpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include <gtest/gtest.h>
#include "../src/Geometry.hpp"

/****************************************************************************/
/*          Run some simple checks on the Point/Line Distance Method        */
/****************************************************************************/
TEST( Geometry, Point_Line_Distance)
{
    // Test 1:  Point and Line Segments are all same point
    ASSERT_LT( Point_Line_Distance( Point<double>( 1.0, 1.0 ), 
                                    Point<double>( 1.0, 1.0 ),
                                    Point<double>( 1.0, 1.0 )), 0.001);

    // Test 2:  Point is different but line segment is the same point
    ASSERT_NEAR( Point_Line_Distance( Point<double>( 1.0, 1.0 ),
                                      Point<double>( 2.0, 1.0 ),
                                      Point<double>( 2.0, 1.0 )), 
                 1.0, 0.001 );

    // Test 3:  Point is on the Line
    ASSERT_NEAR( Point_Line_Distance( Point<double>( 3.0, 1.0 ),
                                      Point<double>( 4.0, 1.0 ),
                                      Point<double>( 1.0, 1.0 )), 
                 0.0, 0.001 );

    // Test 4: Random Point Tests
    Point<double> v1( 2, 1 );
    Point<double> v2( 2, 6 );
    Point<double> v3( 6, 6 );
    Point<double> p1( 1, 6 );
    Point<double> p2( 6, 3 );
    Point<double> p3( 1, 7 );

    ASSERT_NEAR( Point_Line_Distance( p1, v1, v2 ), 1, 0.001 );
    ASSERT_NEAR( Point_Line_Distance( p1, v2, v3 ), 1, 0.001 );
    ASSERT_NEAR( Point_Line_Distance( p2, v1, v2 ), 4, 0.001 );
    ASSERT_NEAR( Point_Line_Distance( p2, v2, v3 ), 3, 0.001 );
    ASSERT_NEAR( Point_Line_Distance( p1, v1, v3 ), 3.90434, 0.001 );
    ASSERT_NEAR( Point_Line_Distance( p2, v1, v3 ), 1.87409, 0.001 );
    ASSERT_NEAR( Point_Line_Distance( p3, v1, v2 ), 1.41421, 0.001 );
    ASSERT_NEAR( Point_Line_Distance( p3, v1, v3 ), 4.52904, 0.001 );
}
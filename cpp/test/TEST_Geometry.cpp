/**
 * @file    TEST_Geometry.hpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include <gtest/gtest.h>
#include "../src/Geometry.hpp"

/************************************************/
/*          Test Linear Interpolation           */
/************************************************/
TEST( Geometry, LERP )
{
    // Test 1: Same Point
    ASSERT_NEAR( Point<double>::Distance_L2( Point<double>::LERP( Point<double>(0,0),
                                                                  Point<double>(0,0),
                                                                  0.5 ),
                                             Point<double>(0,0)),
                 0, 0.0001 );

    // Test 2: Same X Axis
    ASSERT_NEAR( Point<double>::Distance_L2( Point<double>::LERP( Point<double>(10,0),
                                                                  Point<double>(10,10),
                                                                  0.3 ),
                                             Point<double>(10,3)),
                 0, 0.0001 );

    // Test 3: Same Y Axis
    ASSERT_NEAR( Point<double>::Distance_L2( Point<double>::LERP( Point<double>( 0,10),
                                                                  Point<double>(10,10),
                                                                  0.3 ),
                                             Point<double>(3,10)),
                 0, 0.0001 );

    // Test 4: Different Axes
    ASSERT_NEAR( Point<double>::Distance_L2( Point<double>::LERP( Point<double>( 0, 0),
                                                                  Point<double>(10,10),
                                                                  0.3 ),
                                             Point<double>(3,3)),
                 0, 0.0001 );

}

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

/********************************************************/
/*          Test the Segment Density Function           */
/********************************************************/
TEST( Geometry, Get_Segment_Density )
{
    // Create a list of vertices
    std::vector<Point<double>> vertex_list;
    vertex_list.emplace_back( 0, 0 );
    vertex_list.emplace_back( 10, 0 );
    vertex_list.emplace_back( 10, 10 );
    vertex_list.emplace_back( 15, 15 );

    // Create a list of test points
    std::vector<Point<double>> test_points;
    test_points.emplace_back( 2, 0 ); // Should Count on Segment 0-1, step 1,2
    test_points.emplace_back( 9, 0 ); // Should Count on Segment 1-2, step 1
    test_points.emplace_back( 3, 0 ); // Should Count on Segment 0-1, 
    test_points.emplace_back( -10, 10); // Should not count anywhere

    // With a step distance of 4, 
    //   - Segment 0-1 should consist of 3 steps
    //   - Segment 1-2 should consist of 3 steps
    //   - Segment 2-3 should consist of 4 steps
    //   - Segment 3-4 should consist of 2 steps
    ASSERT_NEAR( Get_Segment_Density( vertex_list, test_points, 4 ), 0.5, 0.1 );
}
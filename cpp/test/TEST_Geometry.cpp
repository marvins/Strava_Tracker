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
    ASSERT_LT( Point_Line_Distance( ToPoint2D( 1.0, 1.0 ), 
                                    ToPoint2D( 1.0, 1.0 ),
                                    ToPoint2D( 1.0, 1.0 )), 0.001);

    // Test 2:  Point is different but line segment is the same point
    ASSERT_NEAR( Point_Line_Distance( ToPoint2D( 1.0, 1.0 ),
                                      ToPoint2D( 2.0, 1.0 ),
                                      ToPoint2D( 2.0, 1.0 )), 
                 1.0, 0.001 );

    // Test 3:  Point is on the Line
    ASSERT_NEAR( Point_Line_Distance( ToPoint2D( 3.0, 1.0 ),
                                      ToPoint2D( 4.0, 1.0 ),
                                      ToPoint2D( 1.0, 1.0 )), 
                 0.0, 0.001 );

    // Test 4: Random Point Tests
    auto v1 = ToPoint2D( 2, 1 );
    auto v2 = ToPoint2D( 2, 6 );
    auto v3 = ToPoint2D( 6, 6 );
    auto p1 = ToPoint2D( 1, 6 );
    auto p2 = ToPoint2D( 6, 3 );
    auto p3 = ToPoint2D( 1, 7 );

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
    std::vector<Point> vertex_list;
    vertex_list.push_back( ToPoint2D( 0, 0 ) );
    vertex_list.push_back( ToPoint2D( 10, 0 ) );
    vertex_list.push_back( ToPoint2D( 10, 10 ) );
    vertex_list.push_back( ToPoint2D( 15, 15 ) );

    // Create a list of test points
    std::vector<Point> test_points;
    test_points.push_back( ToPoint2D( 2, 0 )); // Should Count on Segment 0-1, step 1,2
    test_points.push_back( ToPoint2D( 9, 0 )); // Should Count on Segment 1-2, step 1
    test_points.push_back( ToPoint2D( 3, 0 )); // Should Count on Segment 0-1, 
    test_points.push_back( ToPoint2D( -10, 10)); // Should not count anywhere

    // With a step distance of 4, 
    //   - Segment 0-1 should consist of 3 steps
    //   - Segment 1-2 should consist of 3 steps
    //   - Segment 2-3 should consist of 4 steps
    //   - Segment 3-4 should consist of 2 steps
    ASSERT_NEAR( Get_Segment_Density( vertex_list, test_points, 4 ), 2, 0.1 );
}
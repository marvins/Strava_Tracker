/**
 * @file    TEST_Point.cpp
 * @author  Marvin Smith
 * @date    12/23/2020
*/
#include <gtest/gtest.h>

// Project Libraries
#include "../src/Point.hpp"

/********************************************/
/*      Test the Point Constructors         */
/********************************************/
TEST( Point, Constructors )
{
    auto p1 = ToPoint2D( 1, 2 );
    auto p2 = Point();

    ASSERT_NEAR( Point::Distance_L2( p1, ToPoint2D( 1, 2)), 0, 0.01 );
    ASSERT_NEAR( Point::Distance_L2( p2, ToPoint2D( 0, 0)), 0, 0.01 );
}

/****************************************************/
/*          Test the Multiplication Operators       */
/****************************************************/
TEST( Point, Multiplication )
{
    auto p1 = ToPoint2D( 1, 2 );
    auto p2 = p1 * 4;
    auto p3 = 4 * p1;
    auto p4 = ToPoint2D( 4, 8 );

    ASSERT_NEAR( Point::Distance_L2( p2, p4 ), 0, 0.001 );
    ASSERT_NEAR( Point::Distance_L2( p3, p4 ), 0, 0.001 );
}


/********************************************/
/*          Test the Min Function           */
/********************************************/
TEST( Point, Min )
{
    // Define the eps
    const double eps = 0.0001;

    // Create some points (double)
    auto point01 = ToPoint2D( 0, 0 );
    auto point02 = ToPoint2D(1, 0);
    auto point03 = ToPoint2D(0, 1);
    auto point04 = ToPoint2D(1, 1);
    auto point05 = ToPoint2D(-1, 0);
    auto point06 = ToPoint2D(0, -1);
    auto point07 = ToPoint2D(-1, -1);

    // Define expected results
    auto exp_min_1 = ToPoint2D(0,0);
    auto exp_min_2 = ToPoint2D(0,0);
    auto exp_min_3 = ToPoint2D(-1,-1);

    // Compute Min
    auto act_min_1 = Point::Min( point01, point02 );
    auto act_min_2 = Point::Min( point02, point01 );
    auto act_min_3 = Point::Min( point01, point07 );

    // Compare Points
    ASSERT_NEAR( Point::Distance_L2( act_min_1, exp_min_1 ), 0, 0.001 );
    ASSERT_NEAR( Point::Distance_L2( act_min_2, exp_min_2 ), 0, 0.001 );
    ASSERT_NEAR( Point::Distance_L2( act_min_3, exp_min_3 ), 0, 0.001 );
}

/********************************************/
/*          Test the Max Function           */
/********************************************/
TEST( Point, Max )
{
    // Define the eps
    const double eps = 0.0001;

    // Create some points (double)
    auto point01 = ToPoint2D( 0, 0 );
    auto point02 = ToPoint2D(1, 0);
    auto point03 = ToPoint2D(0, 1);
    auto point04 = ToPoint2D(1, 1);
    auto point05 = ToPoint2D(-1, 0);
    auto point06 = ToPoint2D(0, -1);
    auto point07 = ToPoint2D(-1, -1);

    // Define expected results
    auto exp_max_1 = ToPoint2D(1,0);
    auto exp_max_2 = ToPoint2D(1,0);
    auto exp_max_3 = ToPoint2D(0,0);

    // Compute Min
    auto act_max_1 = Point::Max( point01, point02 );
    auto act_max_2 = Point::Max( point02, point01 );
    auto act_max_3 = Point::Max( point01, point07 );

    // Compare Points
    ASSERT_NEAR( Point::Distance_L2( act_max_1, exp_max_1 ), 0, 0.001 );
    ASSERT_NEAR( Point::Distance_L2( act_max_2, exp_max_2 ), 0, 0.001 );
    ASSERT_NEAR( Point::Distance_L2( act_max_3, exp_max_3 ), 0, 0.001 );
}

/************************************************/
/*          Test Linear Interpolation           */
/************************************************/
TEST( Point, LERP )
{
    // Test 1: Same Point
    ASSERT_NEAR( Point::Distance_L2( Point::LERP( ToPoint2D(0,0),
                                                  ToPoint2D(0,0),
                                                  0.5 ),
                                     ToPoint2D(0,0)),
                 0, 0.0001 );

    // Test 2: Same X Axis
    ASSERT_NEAR( Point::Distance_L2( Point::LERP( ToPoint2D(10,0),
                                                  ToPoint2D(10,10),
                                                  0.3 ),
                                     ToPoint2D(10,3)),
                 0, 0.0001 );

    // Test 3: Same Y Axis
    ASSERT_NEAR( Point::Distance_L2( Point::LERP( ToPoint2D( 0,10),
                                                  ToPoint2D(10,10),
                                                  0.3 ),
                                     ToPoint2D(3,10)),
                 0, 0.0001 );

    // Test 4: Different Axes
    ASSERT_NEAR( Point::Distance_L2( Point::LERP( ToPoint2D(0, 0),
                                                  ToPoint2D(10,10),
                                                  0.3 ),
                                     ToPoint2D(3,3)),
                 0, 0.0001 );
}
/**
 * @file    TEST_Rect.cpp
 * @author  Marvin Smith
 * @date    12/23/2020
*/
#include <gtest/gtest.h>

// Project Libraries
#include "../src/Rect.hpp"

/******************************************************/
/*          Test the Rectangle Constructors           */
/******************************************************/
TEST( GEOM_A_Rectangle, Constructors )
{
    Rect rect01;

    ASSERT_NEAR( rect01.TL().x(), 0, 0.0001 );
    ASSERT_NEAR( rect01.TL().y(), 0, 0.0001 );
    ASSERT_NEAR( rect01.TR().x(), 0, 0.0001 );
    ASSERT_NEAR( rect01.TR().y(), 0, 0.0001 );
    ASSERT_NEAR( rect01.BL().x(), 0, 0.0001 );
    ASSERT_NEAR( rect01.BL().y(), 0, 0.0001 );
    ASSERT_NEAR( rect01.BR().x(), 0, 0.0001 );
    ASSERT_NEAR( rect01.BR().y(), 0, 0.0001 );

    ASSERT_NEAR( rect01.Width(), 0, 0.0001 );
    ASSERT_NEAR( rect01.Height(), 0, 0.0001 );

    // Rectangle 2
    Rect rect02( ToPoint2D(1, 2), 3, 4);
    
    ASSERT_EQ( rect02.TL().x(), 1 );
    ASSERT_EQ( rect02.TL().y(), 6 );

    ASSERT_EQ( rect02.TR().x(), 4 );
    ASSERT_EQ( rect02.TR().y(), 6 );

    ASSERT_EQ( rect02.BL().x(), 1 );
    ASSERT_EQ( rect02.BL().y(), 2 );

    ASSERT_EQ( rect02.BR().x(), 4 );
    ASSERT_EQ( rect02.BR().y(), 2 );

    ASSERT_EQ( rect02.Width(), 3 );
    ASSERT_EQ( rect02.Height(), 4 );

    // Rectangle 2
    Rect rect03( ToPoint2D(1, 2), 
                 ToPoint2D(4, 6));

    ASSERT_EQ( rect03.TL().x(), 1 );
    ASSERT_EQ( rect03.TL().y(), 6 );

    ASSERT_EQ( rect03.TR().x(), 4 );
    ASSERT_EQ( rect03.TR().y(), 6 );

    ASSERT_EQ( rect03.BL().x(), 1 );
    ASSERT_EQ( rect03.BL().y(), 2 );

    ASSERT_EQ( rect03.BR().x(), 4 );
    ASSERT_EQ( rect03.BR().y(), 2 );

    ASSERT_EQ( rect03.Width(), 3 );
    ASSERT_EQ( rect03.Height(), 4 );

}

/****************************************/
/*      Test the Is_Inside Function     */
/****************************************/
TEST( Rect, Is_Inside )
{
    Rect rect01( ToPoint2D(1, 1), ToPoint2D(4, 4));

    ASSERT_FALSE( rect01.Is_Inside( ToPoint2D(0,0)));
    ASSERT_TRUE(  rect01.Is_Inside( ToPoint2D(2,2)));
    ASSERT_FALSE( rect01.Is_Inside( ToPoint2D(5,5)));

    // Define the outer rectangle
    Rect test_rectangle( ToPoint2D(-1,-1),
                         ToPoint2D( 2, 2));

    // Test 01 - Empty Rectangle (0,0) -> (0,0) - Should be true
    Rect eval_rect_01;
    ASSERT_TRUE( test_rectangle.Is_Inside( eval_rect_01 ));
    
    // Test 02 - Complete overlap - Should be true
    Rect eval_rect_02( ToPoint2D(-1, -1), ToPoint2D( 2,  2));
    ASSERT_TRUE( test_rectangle.Is_Inside( eval_rect_02 ));
    
    // Test 03 - Completely inside - Should be true
    Rect eval_rect_03( ToPoint2D(-0.5, -0.5), ToPoint2D( 1,  1));
    ASSERT_TRUE( test_rectangle.Is_Inside( eval_rect_03 ));
    
    // Test 04 - Completely inside but on inside corner- Should be true
    Rect eval_rect_04( ToPoint2D(-1, -1), ToPoint2D( 1,  1));
    ASSERT_TRUE( test_rectangle.Is_Inside( eval_rect_04 ));

    // Test 05 - Rectangle is about the same size but barely larger, should be false.
    Rect eval_rect_05( ToPoint2D(-1.0001, -1.0001), ToPoint2D( 2,  2));
    ASSERT_FALSE( test_rectangle.Is_Inside( eval_rect_05 ));
}

/********************************************/
/*      Test the Intersection Operator      */
/********************************************/
TEST( Rect, Intersection )
{
    // Create a rectangle
    Rect rect01( ToPoint2D(2.0,5.0), ToPoint2D(3.0,6.0));
    Rect rect02( ToPoint2D(1.9,4.9), ToPoint2D(3.1,6.1));
    Rect rect03( ToPoint2D(0.0,0.0), ToPoint2D(0.9,0.0));

    // test the first two (Overlap should be exactly the same as rect 01
    auto result01 = Rect::Intersection( rect01, rect02 );
    ASSERT_NEAR( result01.TL().x(), 2.0, 0.0001 );
    ASSERT_NEAR( result01.TR().x(), 3.0, 0.0001 );
    ASSERT_NEAR( result01.BL().y(), 5.0, 0.0001 );
    ASSERT_NEAR( result01.TL().y(), 6.0, 0.0001 );
    ASSERT_NEAR( result01.Area(), 1, 0.0001 );

    auto result02 = Rect::Intersection( rect01, rect03 );
    ASSERT_NEAR( result02.Area(), 0, 0.0001 );


    // Test a real-world case
    Rect rect04( ToPoint2D(513355.706627353, 4300009.386503953), 2500, 2500);
    Rect rect05( ToPoint2D(517471.026472416, 4305667.047952916 ), 454.469, 1449.02);
    Rect result03 = Rect::Intersection(rect04, rect05);

    ASSERT_NEAR( result03.Area(), 0, 0.00001 );
}

/********************************************/
/*          Test the Union Operator         */
/********************************************/
TEST( Rect, Union )
{
    // Create a rectangle
    Rect rect01( ToPoint2D(2.0,5.0), ToPoint2D( 3.0, 6.0));
    Rect rect02( ToPoint2D(1.9,4.9), ToPoint2D( 3.1, 6.1));
    Rect rect03( ToPoint2D(0.0,0.0), ToPoint2D( 0.9, 0.9));

    // test the first two (Overlap should be exactly the same as rect 02
    auto result01 = Rect::Union( rect01, rect02 );
    ASSERT_NEAR( result01.BL().x(), 1.9, 0.0001 );
    ASSERT_NEAR( result01.TR().x(), 3.1, 0.0001 );
    ASSERT_NEAR( result01.BL().y(), 4.9, 0.0001 );
    ASSERT_NEAR( result01.TR().y(), 6.1, 0.0001 );
    ASSERT_NEAR( result01.Area(), 1.4399, 0.0001 );

    auto result02 = Rect::Union( rect01, rect03 );
    ASSERT_NEAR( result02.BL().x(), 0, 0.001 );
    ASSERT_NEAR( result02.BL().y(), 0, 0.001 );
    ASSERT_NEAR( result02.TR().x(), 3, 0.001 );
    ASSERT_NEAR( result02.TR().y(), 6, 0.001 );
}


/*******************************************************/
/*               Test the Area Operator                */
/*******************************************************/
TEST( Rect, Area )
{
    // Create rectangles
    Rect rect01( ToPoint2D( 1.0, 1.0), ToPoint2D(2.0,2.0));
    Rect rect02( ToPoint2D(-1.0,-1.0), ToPoint2D(2.1,2.1));

    ASSERT_NEAR( rect01.Area(), 1, 0.0001 );
    ASSERT_NEAR( rect02.Area(), 9.61, 0.0001 );
}
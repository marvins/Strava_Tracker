/**
 * @file    TEST_GDAL_Utilities.cpp
 * @author  Marvin Smith
 * @author  12/22/2020
*/
#include <gtest/gtest.h>

// Project Libraries
#include "../src/GDAL_Utilities.hpp"

/******************************************************/
/*          Test the UTM to Lat-Lon Conversion        */
/******************************************************/
TEST( GDAL_Utilities, Create_UTM_to_DD_Transformation )
{
    // Create the transformer
    auto xform = Create_UTM_to_DD_Transformation( 32611 );

    auto point = Convert_Coordinate( xform, ToPoint2D( 384409, 4048901 ) );
    ASSERT_NEAR( point.x(), 36.578581, 0.001 );
    ASSERT_NEAR( point.y(), -118.291995, 0.001 );
}

/******************************************************/
/*          Test the Lat-Lon to UTM Conversion        */
/******************************************************/
TEST( GDAL_Utilities, Create_DD_to_UTM_Transformation )
{
    // Create the transformer
    auto xform = Create_DD_to_UTM_Transformation( 32611 );

    auto point = Convert_Coordinate( xform, ToPoint2D( 36.578581, -118.291995 ) );
    ASSERT_NEAR( point.x(), 384409, 1 );
    ASSERT_NEAR( point.y(), 4048901, 1 );
}
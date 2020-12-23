/**
 * @file    TEST_Accumulator.cpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include <gtest/gtest.h>

// Project Libraries
#include "../src/Accumulator.hpp"

// Boost Libraries
#include <boost/log/trivial.hpp>

TEST( Accumulator, Simple_Example )
{
    Accumulator<double> acc;
    
    acc.Insert( -1 );
    acc.Insert( 1.2 );
    for( int i=0; i<100; i++ )
    {
        acc.Insert( (rand() % 100)/50.0 - 1.0 );
    }

    // Measure Values
    ASSERT_NEAR( acc.Get_Min(), -1, 0.0001 );
    ASSERT_NEAR( acc.Get_Max(), 1.2, 0.00001 );
    ASSERT_NEAR( acc.Get_Mean(), 0, 0.2 );
    ASSERT_EQ( acc.Get_Count(), 102 );

    BOOST_LOG_TRIVIAL(debug) << acc.To_String( "Test Accumulator", "ms" );
}
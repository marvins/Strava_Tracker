/**
 * @file    TEST_WaypointList.cpp
 * @author  Marvin Smith
 * @date    12/21/2020
*/
#include <gtest/gtest.h>

// Project Libraries
#include "../src/WaypointList.hpp"

// Boost Libraries
#include <boost/log/trivial.hpp>

/****************************************************************/
/*          Test the Waypoint Randomize Vector Method           */
/****************************************************************/
TEST( WaypointList, Randomize_Vertices )
{
    auto wp1 = WaypointList( "08210299021903430015092402001517010008700195058601241099029719860700234208460033", 
                             10, 867, 2326, 
                             ToPoint2D(0,0), 
                             ToPoint2D(867, 2326) );
    auto verts = wp1.Get_Vertices();
    //BOOST_LOG_TRIVIAL(debug) << "Pre-Randomize: " << wp1.To_String(true);
    auto wp2 = wp1;

    for( int i=0; i<5; i++ )
    {
        wp2.Randomize_Vertices( wp2 );
        //BOOST_LOG_TRIVIAL(debug) << "Post-Randomize: " << wp2.To_String(true);
    }
}

/*********************************************************/
/*          Test the Waypoint Randomize Method           */
/*********************************************************/
TEST( WaypointList, Randomize )
{
    auto wp1 = WaypointList( "08210299021903430015092402001517010008700195058601241099029719860700234208460033", 
                             10, 867, 2326, 
                             ToPoint2D(0,0), 
                             ToPoint2D(867, 2326) );
    auto verts = wp1.Get_Vertices();
    //BOOST_LOG_TRIVIAL(debug) << "Pre-Randomize: " << wp1.To_String(true);
    auto wp2 = wp1;

    for( int i=0; i<5; i++ )
    {
        WaypointList::Randomize( wp2 );
        BOOST_LOG_TRIVIAL(debug) << "Post-Randomize: " << wp2.To_String(true);
    }
}
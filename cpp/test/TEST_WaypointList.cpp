/**
 * @file    TEST_WaypointList.cpp
 * @author  Marvin Smith
 * @date    12/21/2020
*/
#include <gtest/gtest.h>

// C++ Libraries
#include <filesystem>

// Project Libraries
#include "../src/Context.hpp"
#include "../src/DB_Utils.hpp"
#include "../src/WaypointList.hpp"
#include "Utilities.hpp"

// Boost Libraries
#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>

/*************************************************************/
/*          Test the Waypoint Create_Random Method           */
/*************************************************************/
TEST( WaypointList, Create_Random )
{
    auto wp1 = WaypointList::Create_Random( 10, 867, 2326, 
                                            ToPoint2D(0,0), 
                                            ToPoint2D(867, 2326) );
    ASSERT_EQ( wp1.Get_DNA().size(), 10 * 7 );
}

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

/************************************************************/
/*          Test the WaypointList Fitness Method            */
/************************************************************/
TEST( WaypointList, Update_Fitness )
{
    // Path to Unit-Test Data
    std::filesystem::path db_path( "cpp/unit_test_data/bike_data.db" );
    if( !std::filesystem::is_regular_file( db_path ) )
    {
        BOOST_LOG_TRIVIAL(error) << "Test Database Path Does Not Exist: " << db_path;
        FAIL();
    }
    std::filesystem::path coord_path( "./cpp/unit_test_data/sector_2_test_route.csv" );
    if( !std::filesystem::is_regular_file( coord_path ) )
    {
        BOOST_LOG_TRIVIAL(error) << "Coordinate Path Does Not Exist: " << coord_path;
        FAIL();
    }

    // Sector Info
    int epsg_code = 32613;
    auto start_point = ToPoint2D( 5.707200, 1.696290 );
    auto end_point   = ToPoint2D( 545.149380, 1441.971723 );

    // Load the database
    sqlite3 *db;
    auto rc = sqlite3_open( db_path.c_str(), &db );
    ASSERT_EQ( rc, 0 );

    // For Each Sector, Load the points
    int sector_id = 2;
    auto point_list = Load_Point_List( db, sector_id );
    ASSERT_GT( point_list.size(), 1000 );

    // Normalize to get standard range
    auto range = Normalize_Points( point_list );
    size_t max_x = std::get<2>(range) - std::get<0>(range) + 1;
    size_t max_y = std::get<3>(range) - std::get<1>(range) + 1;
    BOOST_LOG_TRIVIAL(debug) << "Min: [" << std::get<0>(range) << ", " << std::get<1>(range) 
                             << "], Max: [" << std::get<2>(range) << ", " << std::get<3>(range) << "]";

    // Load the test coordinate list
    auto vertices = Load_CSV_Vertices( coord_path, epsg_code );
    for( auto& v : vertices )
    {
        v.x() -= std::get<0>(range);
        v.y() -= std::get<1>(range);
    }

    // Create Quad Tree
    Rect point_bbox( ToPoint2D( -10, -10 ),
                     std::get<2>(range) - std::get<0>(range) + 20,
                     std::get<3>(range) - std::get<1>(range) + 20);
    int max_objects = 8;
    int max_levels = 10;

    // Create Context Object
    Context context;
    context.point_list = point_list;
    context.density_step_distance = 25;
    context.start_point = start_point;
    context.end_point   = end_point;
    context.point_quad_tree = QuadTree<QTNode>( point_bbox, max_objects, max_levels );
    for( const auto& pt : context.point_list )
    {
        context.geo_point_list.push_back( ToPoint2D( pt.x_norm, pt.y_norm ) );
        auto node = std::make_shared<QTNode>( pt.index, context.geo_point_list.back() );
        context.point_quad_tree.Insert( node );
    }

    // Create the WaypointList
    auto wp_test = WaypointList( vertices, 
                                 max_x,
                                 max_y,
                                 start_point,
                                 end_point );

    // Get the fitness score
    Stats_Aggregator aggregator;
    wp_test.Update_Fitness( &context, false, aggregator );
    BOOST_LOG_TRIVIAL(debug) << wp_test.To_String(true);

    // Show the reference to try and beat
    auto wp_ref = WaypointList( "00860049035902700795058710000760104208990990096309971000099910011127100013421000122907841209081511770869110509120999108809991091100011000939125109951271099912721000127209451268094012720919128008991280087312790799126407901269077712680970119312890972126009461199088112130770096906790939067909430698098908841013105207691189",
                                wp_test.Get_Number_Waypoint(),
                                wp_test.Get_Max_X(),
                                wp_test.Get_Max_Y(),
                                wp_test.Get_Start_Point(),
                                wp_test.Get_End_Point() );

    // Get the fitness score
    wp_ref.Update_Fitness( &context, false, aggregator );
    BOOST_LOG_TRIVIAL(debug) << wp_ref.To_String(true);

    // Cleanup
    sqlite3_close(db);
}
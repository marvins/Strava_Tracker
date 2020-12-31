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
#include "../src/GDAL_Utilities.hpp"
#include "../src/WaypointList.hpp"
#include "../src/Write_Worker.hpp"
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
    std::filesystem::path coord_path( "./cpp/unit_test_data/sector_2_edge_cases.csv" );
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
    std::string sector_id = "sector_2";
    auto point_list = Load_Point_List( db, sector_id );
    ASSERT_GT( point_list.size(), 1000 );

    // Normalize to get standard range
    auto range = Normalize_Points( point_list );
    size_t max_x = std::get<2>(range) - std::get<0>(range) + 1;
    size_t max_y = std::get<3>(range) - std::get<1>(range) + 1;
    BOOST_LOG_TRIVIAL(debug) << "Min: [" << std::get<0>(range) << ", " << std::get<1>(range) 
                             << "], Max: [" << std::get<2>(range) << ", " << std::get<3>(range) << "]";

    // Create Context Object
    Context context;
    context.point_list = point_list;
    context.start_point = start_point;
    context.end_point   = end_point;
    for( const auto& pt : context.point_list )
    {
        context.geo_point_list.push_back( ToPoint2D( pt.x_norm, pt.y_norm ) );
    }

    // Load the unit-test fitness data
    auto fitness_samples = Load_CSV_Fitness_Samples( coord_path );

    // Create Reference Sample
    Stats_Aggregator aggregator( "junk_path" );
    aggregator.Start_Writer();
    auto ref_wp = WaypointList( std::get<1>(fitness_samples[0]), 
                                std::get<2>(fitness_samples[0]),
                                max_x,
                                max_y,
                                start_point,
                                end_point );

    ref_wp.Update_Fitness( &context, 
                           false,
                           aggregator );
    BOOST_LOG_TRIVIAL(debug) << "Reference Waypoint: " << ref_wp.To_String(true);
    
    // Create Test Samples
    for( size_t i=1; i<fitness_samples.size(); i++ )
    {
        // Create waypointlist
        BOOST_LOG_TRIVIAL(debug) << "Processing Sample: " << std::get<0>(fitness_samples[i]);
        auto wp = WaypointList( std::get<1>(fitness_samples[i]), 
                                std::get<2>(fitness_samples[i]),
                                max_x,
                                max_y,
                                start_point,
                                end_point );
        
        // Compute Fitness Update
        wp.Update_Fitness( &context, 
                           false,
                           aggregator );
        
        BOOST_LOG_TRIVIAL(debug) << wp.To_String(true);
        ASSERT_LT( ref_wp.Get_Fitness(), wp.Get_Fitness() );
    }

    // Cleanup
    sqlite3_close(db);
}

/********************************************************************/
/*          Test the WaypointList Seed-Population Method            */
/********************************************************************/
TEST( WaypointList, Seed_Population )
{
    // Path to Unit-Test Data
    std::filesystem::path db_path( "cpp/unit_test_data/bike_data.db" );
    if( !std::filesystem::is_regular_file( db_path ) )
    {
        BOOST_LOG_TRIVIAL(error) << "Test Database Path Does Not Exist: " << db_path;
        FAIL();
    }

    // Important Info
    auto start_point = ToPoint2D( 5.707200, 1.696290 );
    auto end_point   = ToPoint2D( 545.149380, 1441.971723 );
    size_t min_waypoints = 6;
    size_t max_waypoints = 14;
    size_t population_size = 50;

    auto xform_utm2dd = Create_UTM_to_DD_Transformation( 32613 );

    // Load the database
    sqlite3 *db;
    auto rc = sqlite3_open( db_path.c_str(), &db );
    ASSERT_EQ( rc, 0 );

    // For Sector, Load the points
    std::string sector_id = "sector_2";
    int dataset_id = 2;

    auto point_list = Load_Point_List( db, sector_id );

    // Normalize to get standard range
    auto range = Normalize_Points( point_list );
    size_t max_x = std::get<2>(range) - std::get<0>(range) + 1;
    size_t max_y = std::get<3>(range) - std::get<1>(range) + 1;
    BOOST_LOG_TRIVIAL(debug) << "Min: [" << std::get<0>(range) << ", " << std::get<1>(range) 
                             << "], Max: [" << std::get<2>(range) << ", " << std::get<3>(range) << "]";

    // Create Context Object
    Context context;
    context.point_list = point_list;
    context.start_point = start_point;
    context.end_point   = end_point;
    for( const auto& pt : context.point_list )
    {
        context.geo_point_list.push_back( ToPoint2D( pt.x_norm, pt.y_norm ) );
    }

    // Create the seeded population
    auto seed_db_point_list = Load_Point_List( db, sector_id, dataset_id );
    Normalize_Points( seed_db_point_list, 
                      std::get<0>(range),
                      std::get<1>(range) );
    std::vector<Point> seed_point_list;
    for( const auto& pt : seed_db_point_list )
    {
        seed_point_list.push_back( ToPoint2D( pt.x_norm, pt.y_norm ) );
    }

    auto seeded_population = Seed_Population( seed_point_list,
                                              min_waypoints,
                                              max_waypoints,
                                              population_size,
                                              max_x,
                                              max_y,
                                              start_point,
                                              end_point );

    ASSERT_EQ( seeded_population.size(), max_waypoints - min_waypoints + 1 );
    for( size_t i=min_waypoints; i<= max_waypoints; i++ )
    {
        ASSERT_EQ( seeded_population[i].size(), population_size );
    }

    // Write Point Data to Disk
    std::map<std::string,std::map<int,std::map<int,std::vector<DB_Point>>>> master_vertex_list;
    auto writer_obj = std::make_shared<Write_Worker>( xform_utm2dd,
                                                      range,
                                                      point_list.front().gz,
                                                      master_vertex_list );

    // Get the fitness score
    Stats_Aggregator aggregator( "junk_path" );
    aggregator.Start_Writer();
    size_t counter = 0;
    for( size_t sector=0; sector<5; sector++ )
    {
        for( auto& pr : seeded_population )
        {
            for( auto& p : pr.second )
            {
                p.Update_Fitness( &context, false, aggregator );
            }
            writer_obj->Write( pr.second.front(), 
                               "sector_" + std::to_string(sector), 
                               counter++ );
        }
    }

    // Cleanup
    sqlite3_close(db);
}
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
    context.density_step_distance = 10;
    context.start_point = start_point;
    context.end_point   = end_point;
    for( const auto& pt : context.point_list )
    {
        context.geo_point_list.push_back( ToPoint2D( pt.x_norm, pt.y_norm ) );
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
    BOOST_LOG_TRIVIAL(debug) << "Proposed Best WaypointList:\n" << wp_test.To_String(true);

    // Show the reference to try and beat
    auto wp_ref = WaypointList( "00460010006100180067002401350081018601230295021003680266054804040651048207070516072005230764055407780566078605730835062108490633091806650951067711020722117907581186076111970767120307711237079412710876128609071299092311390886104409171006093110091032101110391002108709771160096412030903127408901274075812620709127707021279",
                                wp_test.Get_Number_Waypoint(),
                                wp_test.Get_Max_X(),
                                wp_test.Get_Max_Y(),
                                wp_test.Get_Start_Point(),
                                wp_test.Get_End_Point() );

    // Get the fitness score
    wp_ref.Update_Fitness( &context, false, aggregator );
    BOOST_LOG_TRIVIAL(debug) << "Reference WaypointList:\n" << wp_ref.To_String(true);

    // Test a bad point
    //auto wp_bad = WaypointList( "01410085015800990163010401690107035702500368026604600340046603470753054407570547076005500782056920690708107907141082057908220608091806651013069711370739115507471169075312650848128609071329094912170899106109151011104310111060100910681002108709701167097301910966120108831273087612760860127608041256066512840666129205621417",
    auto wp_bad = WaypointList( "12490551274486430965018006271377019807419896021600241336075012910288065811451032131305610459080807260396091513151301091906660172052506861225112701490050003201090727044310711284049901421312101511180047016100928293828506860232040118181162055810401217031900961085136609341152083003440274050910601088078810280809126939801041",
                                 wp_test.Get_Number_Waypoint(),
                                 wp_test.Get_Max_X(),
                                 wp_test.Get_Max_Y(),
                                 wp_test.Get_Start_Point(),
                                 wp_test.Get_End_Point() );

    // Get the fitness score
    wp_bad.Update_Fitness( &context, false, aggregator );
    BOOST_LOG_TRIVIAL(debug) << "Bad WaypointList:\n" << wp_bad.To_String(true);

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
    size_t min_waypoints = 20;
    size_t max_waypoints = 29;
    size_t population_size = 50;

    auto xform_utm2dd = Create_UTM_to_DD_Transformation( 32613 );

    // Load the database
    sqlite3 *db;
    auto rc = sqlite3_open( db_path.c_str(), &db );
    ASSERT_EQ( rc, 0 );

    // For Sector, Load the points
    int sector_id = 2;
    int dataset_id = 2;

    auto point_list = Load_Point_List( db, sector_id );

    // Normalize to get standard range
    auto range = Normalize_Points( point_list );
    size_t max_x = std::get<2>(range) - std::get<0>(range) + 1;
    size_t max_y = std::get<3>(range) - std::get<1>(range) + 1;
    BOOST_LOG_TRIVIAL(debug) << "Min: [" << std::get<0>(range) << ", " << std::get<1>(range) 
                             << "], Max: [" << std::get<2>(range) << ", " << std::get<3>(range) << "]";


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
    std::map<int,std::vector<DB_Point>> master_vertex_list;
    auto writer_obj = std::make_shared<Write_Worker>( xform_utm2dd,
                                                      range,
                                                      point_list.front().gz,
                                                      master_vertex_list );

    // Get the fitness score
    Stats_Aggregator aggregator;
    for( auto& pr : seeded_population )
    {
        for( auto& p : pr.second )
        {
            p.Update_Fitness( &context, false, aggregator );
        }
        writer_obj->Write( pr.second.front(), 0 );
    }

    // Cleanup
    sqlite3_close(db);
}
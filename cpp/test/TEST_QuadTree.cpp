/**
 * @file    TEST_QuadTree.cpp
 * @author  Marvin Smith
 * @date    12/23/2020
*/
#include <gtest/gtest.h>

// Project Libraries
#include "../src/DB_Utils.hpp"
#include "../src/QuadTree.hpp"

// C++ Libraries
#include <exception>

// Boost Libraries
#include <boost/log/trivial.hpp>

struct Node
{
    Point location;
}; 

/****************************************************/
/*          Sample usage of the quad tree           */
/****************************************************/
TEST( QuadTree, Small_Scale_Usage )
{
    // Create an empty quad-tree
    QuadTree<QTNode> qt( Rect( ToPoint2D(-10, -10), 20, 20 ) );

    // For this test, we need a small, specific set of points
    size_t counter = 0;
    for( int i=1; i<=10; i++ )
    {
        qt.Insert( std::make_shared<QTNode>( counter++, ToPoint2D( -i, -i ) ) );
        qt.Insert( std::make_shared<QTNode>( counter++, ToPoint2D( -i,  i ) ) );
        qt.Insert( std::make_shared<QTNode>( counter++, ToPoint2D(  i, -i ) ) );
        qt.Insert( std::make_shared<QTNode>( counter++, ToPoint2D(  i,  i ) ) );
    }

    // Make sure going out of bounds is caught
    ASSERT_THROW( qt.Insert( std::make_shared<QTNode>( counter++, ToPoint2D( 11, 9) ) ), std::invalid_argument );

    // Query for all nodes inside region
    //BOOST_LOG_TRIVIAL(debug) << qt.To_String();
    ASSERT_EQ( qt.Search( ToPoint2D(  0, 0 ), 1.5 ).size(), 4 ); // Test all of the tree
    ASSERT_EQ( qt.Search( ToPoint2D( -8, 6 ), 3 ).size(), 3 ); // Test NW Edge of tree
}

/****************************************************/
/*          Build QuadTree with all Points          */
/****************************************************/
TEST( QuadTree, Large_Scale_Usage )
{
    // Load the database
    sqlite3 *db;
    auto rc = sqlite3_open( "cpp/unit_test_data/bike_data.db", &db );
    ASSERT_EQ( rc, 0 );

    // For Each Sector, Load the points
    auto point_list = Load_Point_List( db, "" );
    ASSERT_GT( point_list.size(), 5000 );

    // Normalize to get standard range
    auto range = Normalize_Points( point_list );

    // Compute bounding box
    Rect bbox( ToPoint2D( -10, -10 ),
               std::get<2>(range) - std::get<0>(range) + 20,
               std::get<3>(range) - std::get<1>(range) + 20);
    BOOST_LOG_TRIVIAL(debug) << "Loaded " << point_list.size() << " points, Range: " << bbox.To_String();

    // Create Quad Tree
    int max_objects = 20;
    int max_levels = 10;
    QuadTree<QTNode> qt( bbox, 
                         max_objects,
                         max_levels );

    // Insert Points into QuadTree
    for( const auto& point : point_list )
    {
        qt.Insert( std::make_shared<QTNode>( point.index, ToPoint2D( point.x_norm, point.y_norm ) ));
    }

    // Perform some big queries
    auto sector_1_stop = ToPoint2D( 511210 - std::get<0>(range),
                                    4388378 - std::get<1>(range) );
    auto results = qt.Search( sector_1_stop, 20 );
    ASSERT_EQ( results.size(), 16 );

    // Cleanup
    sqlite3_close(db);
}
/**
 * @file    TEST_Geometry.hpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include <gtest/gtest.h>

// C++ Libraries
#include <chrono>

// Project Libraries
#include "../src/Accumulator.hpp"
#include "../src/DB_Utils.hpp"
#include "../src/Geometry.hpp"
#include "../src/QuadTree.hpp"

// Boost Libraries
#include <boost/log/trivial.hpp>

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
    // Load the database
    sqlite3 *db;
    auto rc = sqlite3_open( "cpp/unit_test_data/bike_data.db", &db );
    ASSERT_EQ( rc, 0 );

    // For Each Sector, Load the points
    auto point_list = Load_Point_List( db, -1 );
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

    // Create an Accumulator
    std::map<int,Accumulator<double>> acc, density_acc;

    // Create a vertex list
    std::vector<Point> vertex_list;
    vertex_list.push_back( ToPoint2D( 776.628614, 3.773305 ));
    vertex_list.push_back( ToPoint2D( 870.000000, 279.000000 ));
    vertex_list.push_back( ToPoint2D(  184.000000, 360.000000 ));
    vertex_list.push_back( ToPoint2D(  175.000000, 797.000000 ));
    vertex_list.push_back( ToPoint2D(  23.000000, 900.000000 ));
    vertex_list.push_back( ToPoint2D(  279.000000, 1891.000000 ));
    vertex_list.push_back( ToPoint2D(  432.000000, 2143.000000 ));
    vertex_list.push_back( ToPoint2D(  719.707200, 2317.696290 ));
    //BOOST_LOG_TRIVIAL(debug) << qt.To_String();

    int step_distance = 25;
    for( size_t i=0; i<10000; i++ )
    {
        auto start_time = std::chrono::steady_clock::now();
        double density = Get_Segment_Density( vertex_list, qt, step_distance );
        auto stop_time = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - start_time ).count()/1000.0;
        acc[step_distance].Insert( stop_time );
        density_acc[step_distance].Insert( density );
    }
    for( const auto& a : acc )
    {
        BOOST_LOG_TRIVIAL(debug) << a.second.To_String( "Timing Step Distance: " + std::to_string(a.first), "sec");
        BOOST_LOG_TRIVIAL(debug) << density_acc[a.first].To_String( "Density Step Distance: " + std::to_string(a.first), "" );
    }

    // Cleanup
    sqlite3_close(db);
}
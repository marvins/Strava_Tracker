/**
 * @file    TEST_DB_Utils.cpp
 * @author  Marvin Smith
 * @date    12/26/2020
 */
#include <gtest/gtest.h>

// C++ Libraries
#include <filesystem>

// Boost Libraries
#include <boost/log/trivial.hpp>

// Project Libraries
#include "../src/DB_Utils.hpp"

/****************************************************/
/*          Test the Load Point List Method         */
/****************************************************/
TEST( DB_Utils, Load_Point_List )
{
    // Path to Unit-Test Data
    std::filesystem::path db_path( "cpp/unit_test_data/bike_data.db" );
    if( !std::filesystem::is_regular_file( db_path ) )
    {
        BOOST_LOG_TRIVIAL(error) << "Test Database Path Does Not Exist: " << db_path;
        FAIL();
    }

    // Load the database
    sqlite3 *db;
    auto rc = sqlite3_open( db_path.c_str(), &db );
    ASSERT_EQ( rc, 0 );

    auto point_list_full = Load_Point_List( db, "", -1 );
    ASSERT_EQ( point_list_full.size(), 15773 );

    auto point_list_s1 = Load_Point_List( db, "sector_1" );
    ASSERT_EQ( point_list_s1.size(), 1962 );

    auto point_list_s1_d2 = Load_Point_List( db, "sector_1", 2 );
    ASSERT_EQ( point_list_s1_d2.size(), 535 );

    // Cleanup
    sqlite3_close(db);
}

/*****************************************************/
/*          Test the Load Sector Data Method         */
/*****************************************************/
TEST( DB_Utils, Load_Sector_Data )
{
    // Path to Unit-Test Data
    std::filesystem::path db_path( "cpp/unit_test_data/bike_data.db" );
    if( !std::filesystem::is_regular_file( db_path ) )
    {
        BOOST_LOG_TRIVIAL(error) << "Test Database Path Does Not Exist: " << db_path;
        FAIL();
    }

    // Load the database
    sqlite3 *db;
    auto rc = sqlite3_open( db_path.c_str(), &db );
    ASSERT_EQ( rc, 0 );

    auto sector_data = Load_Sector_Data( db );
    
    ASSERT_EQ( sector_data.size(), 9 );
    ASSERT_NEAR( Point::Distance_L2( std::get<0>(sector_data["sector_0"]).Get_LLA_Coordinate(), ToPoint2D( 39.598926, -104.860817 ) ), 0, 0.001 );
    ASSERT_NEAR( Point::Distance_L2( std::get<1>(sector_data["sector_8"]).Get_LLA_Coordinate(), ToPoint2D( 39.75488375, -105.00005525 ) ), 0, 0.001 );

    // Cleanup
    sqlite3_close(db);
}
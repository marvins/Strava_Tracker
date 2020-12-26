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

    auto point_list_full = Load_Point_List( db, -1, -1 );
    ASSERT_EQ( point_list_full.size(), 15773 );

    auto point_list_s1 = Load_Point_List( db, 1 );
    ASSERT_EQ( point_list_s1.size(), 1962 );

    auto point_list_s1_d2 = Load_Point_List( db, 1, 2 );
    ASSERT_EQ( point_list_s1_d2.size(), 535 );

    // Cleanup
    sqlite3_close(db);
}
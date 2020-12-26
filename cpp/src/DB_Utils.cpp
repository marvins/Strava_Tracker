/**
 * @file    DB_Utils.cpp
 * @author  Marvin Smith
 * @date    12/19/2020
 */
#include "DB_Utils.hpp"

// C++ Libraries
#include <functional>
#include <iostream>

// Boost Libraries
#include <boost/log/trivial.hpp>

static std::vector<std::string> sector_list;

/************************************/
/*          Callback Worker         */
/************************************/
static int sector_callback( void *data, int argc, char **argv, char **azColName )
{
    for( int i = 0; i<argc; i++ )
    {
        if( azColName[i] == std::string("sector_id") )
        {
            sector_list.push_back( argv[i] );
        }
    }
    return 0;
}

/************************************/
/*          Callback Worker         */
/************************************/
static int point_callback( void *data, int argc, char **argv, char **azColName )
{
    static int counter = 0;
    auto point_list = reinterpret_cast<std::vector<DB_Point>*>( data );
    DB_Point new_point;
    for( int i = 0; i<argc; i++ )
    {
        // Check for Null
        if( !argv[i] )
        {

        }
        else if( azColName[i] == std::string("index") )
        {
            new_point.index = std::stoi( argv[i] );
        }
        else if( azColName[i] == std::string("latitude") )
        {
            new_point.latitude = std::stod( argv[i] );
        }
        else if( azColName[i] == std::string("longitude" ) )
        {
            new_point.longitude = std::stod( argv[i] );
        }
        else if( azColName[i] == std::string("gridZone") )
        {
            new_point.gz = std::stoi( argv[i] );
        }
        else if( azColName[i] == std::string("easting") )
        {
            new_point.easting = std::stod( argv[i] );
        }
        else if( azColName[i] == std::string("northing" ) )
        {
            new_point.northing = std::stod( argv[i] );
        }
        else if( azColName[i] == std::string("sectorId") )
        {
            new_point.sectorId = argv[i];
        }
        else if( azColName[i] == std::string("datasetId") )
        {
            new_point.datasetId = argv[i];
        }
    }
    point_list->push_back( new_point );
    return 0;
}

/********************************/
/*      Load Sector IDs         */
/********************************/
std::vector<std::string>  Load_Sector_IDs( sqlite3 *db )
{
    // Create Statement
    std::string sql = "SELECT sector_id FROM sector_list";
    char* zErrMsg = 0;

    // Make SQL Statement
    auto rc = sqlite3_exec( db, sql.c_str(), sector_callback, nullptr, &zErrMsg );

    // Check Errors 
    if( rc != SQLITE_OK )
    {
        BOOST_LOG_TRIVIAL(error) << "Sector-List SQL Error: " << zErrMsg;
        sqlite3_free( zErrMsg );
    }
    else
    {
      BOOST_LOG_TRIVIAL(debug) << "Sector List Constructed Successfully";
    }

    return sector_list;
}

/****************************************/
/*          Loading Point List          */
/****************************************/
std::vector<DB_Point> Load_Point_List( sqlite3* db, int sector_id, int dataset_id )
{
    // Create Statement
    std::string sql = "SELECT * FROM point_list";
    if( sector_id >= 0 )
    {
        sql += " WHERE sectorID=\"sector_" + std::to_string(sector_id) + "\"";
        if( dataset_id >= 0 )
        {
            sql += " AND datasetId=\"" + std::to_string(dataset_id) + "\"";
        }
    }
    else
    {
        if( dataset_id >= 0 )
        {
            sql += " WHERE datasetId=\"" + std::to_string(dataset_id) + "\"";
        }
    }
    sql += " ORDER BY timestamp";
    
    char* zErrMsg = 0;

    // Make SQL Statement
    std::vector<DB_Point> point_list;
    auto rc = sqlite3_exec( db, sql.c_str(), point_callback, &point_list, &zErrMsg );
    BOOST_LOG_TRIVIAL(debug) << "Finished loading " << point_list.size() << " points. SQL(" + sql + ")";

    // Check Errors 
    if( rc != SQLITE_OK )
    {
        BOOST_LOG_TRIVIAL(error) << "Point-List SQL Error: " << zErrMsg;
        sqlite3_free( zErrMsg );
        std::exit(1);
    }
    else
    {
        BOOST_LOG_TRIVIAL(debug) << "Point-List Loaded Successfully";
    }

    return point_list;
}
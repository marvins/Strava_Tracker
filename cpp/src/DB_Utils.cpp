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
static int sector_point_callback( void *data, int argc, char **argv, char **azColName )
{
    auto sector_point_list = reinterpret_cast<std::map<std::string,std::tuple<DB_Point,DB_Point>>*>( data );
    std::string sector_name;
    DB_Point start_point;
    DB_Point stop_point;

    for( int i = 0; i<argc; i++ )
    {
        if( azColName[i] == std::string("sectorId") )
        {
            sector_name = argv[i];
        }
        else if( azColName[i] == std::string("startLatitude") )
        {
            start_point.latitude = std::stod( argv[i] );
        }
        else if( azColName[i] == std::string("startLongitude") )
        {
            start_point.longitude = std::stod( argv[i] );
        }
        else if( azColName[i] == std::string("startEasting") )
        {
            start_point.easting = std::stod( argv[i] );
        }
        else if( azColName[i] == std::string("startNorthing") )
        {
            start_point.northing = std::stod( argv[i] );
        }
        else if( azColName[i] == std::string("stopLatitude") )
        {
            stop_point.latitude = std::stod( argv[i] );
        }
        else if( azColName[i] == std::string("stopLongitude") )
        {
            stop_point.longitude = std::stod( argv[i] );
        }
        else if( azColName[i] == std::string("stopEasting") )
        {
            stop_point.easting = std::stod( argv[i] );
        }
        else if( azColName[i] == std::string("stopNorthing") )
        {
            stop_point.northing = std::stod( argv[i] );
        }
    }
    (*sector_point_list)[sector_name] = std::make_tuple( start_point, stop_point );
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

/****************************************/
/*          Open the Database           */
/****************************************/
sqlite3* Open_Database( const std::filesystem::path& db_path )
{
    sqlite3 *db;
    auto rc = sqlite3_open( db_path.c_str(), &db );

    // Make sure database is open
    if( rc )
    {
        BOOST_LOG_TRIVIAL(error) << "Can't open the database. Why: " << sqlite3_errmsg(db);
        std::exit(1);
    }
    else
    {
      BOOST_LOG_TRIVIAL(error) << "Opened the database successfully";
    }
    return db;
}

/********************************/
/*      Load Sector IDs         */
/********************************/
std::map<std::string,std::tuple<DB_Point,DB_Point>>  Load_Sector_Data( sqlite3 *db )
{
    // Step 1: Load the list of sector names
    // Create Statement
    std::vector<std::string> sector_list;
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

    // Step 2: Load the list of coordinates
    std::map<std::string,std::tuple<DB_Point,DB_Point>> sector_data;
    sql = "SELECT * FROM sector_point_list";
    rc = sqlite3_exec( db, sql.c_str(), sector_point_callback, &sector_data, &zErrMsg );

    // Check Errors 
    if( rc != SQLITE_OK )
    {
        BOOST_LOG_TRIVIAL(error) << "Sector-Point-List SQL Error: " << zErrMsg;
        sqlite3_free( zErrMsg );
    }
    else
    {
      BOOST_LOG_TRIVIAL(debug) << "Sector List Constructed Successfully";
    }

    return sector_data;
}

/****************************************/
/*          Loading Point List          */
/****************************************/
std::vector<DB_Point> Load_Point_List( sqlite3*           db, 
                                       const std::string& sector_id, 
                                       int                dataset_id )
{
    // Create Statement
    std::string sql = "SELECT * FROM point_list";
    if( !sector_id.empty() )
    {
        sql += " WHERE sectorID=\"" + sector_id + "\"";
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
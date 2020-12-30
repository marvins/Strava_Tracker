/**
 * @file    DB_Utils.hpp
 * @author  Marvin Smith
 * @date    12/19/2020
 */
#pragma once

// SQLite Library
#include <sqlite3.h>

// C++ Libraries
#include <filesystem>
#include <map>
#include <string>
#include <vector>

// Project Libraries
#include "DB_Point.hpp"

/**
 * @brief Open Database
 */
sqlite3* Open_Database( const std::filesystem::path& db_path );

/**
 * @brief Load the Sector ID List
 */
std::map<std::string,std::tuple<DB_Point,DB_Point>>  Load_Sector_Data( sqlite3 *db );

/**
 * @brief Load the Point List
 */
std::vector<DB_Point> Load_Point_List( sqlite3*           db, 
                                       const std::string& sector_id, 
                                       int                dataset_id = -1 );
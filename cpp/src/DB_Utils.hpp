/**
 * @file    DB_Utils.hpp
 * @author  Marvin Smith
 * @date    12/19/2020
 */
#pragma once

// SQLite Library
#include <sqlite3.h>

// C++ Libraries
#include <string>
#include <vector>

// Project Libraries
#include "DB_Point.hpp"

/**
 * @brief Load the Sector ID List
 */
std::vector<std::string>  Load_Sector_IDs( sqlite3 *db );

/**
 * @brief Load the Point List
 */
std::vector<DB_Point> Load_Point_List( sqlite3* db, int sector_id );
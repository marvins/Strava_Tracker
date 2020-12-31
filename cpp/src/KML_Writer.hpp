/**
 * @file    KML_Writer.hpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#pragma once

// C++ Libraries
#include <map>
#include <string>
#include <vector>

// Project Libraries
#include "DB_Point.hpp"

/**
 * @brief Write KML File
 * 
 * @param pathname
 * @param point_list
 */
void Write_KML( const std::string&                                                             pathname,
                const std::map<std::string,std::map<int,std::map<int,std::vector<DB_Point>>>>& point_list );

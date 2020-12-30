/**
 * @file    Utilities.hpp
 * @author  Marvin Smith
 * @date    12/25/2020
 */
#pragma once

// C++ Libraries
#include <filesystem>
#include <map>
#include <string>
#include <vector>

// Project Libraries
#include "../src/Point.hpp"

/**
 * @brief Load the CSV Vertices
 */
std::vector<Point> Load_CSV_Vertices( const std::filesystem::path& coord_path,
                                      int                          epsg_code );

/**
 * @brief Load the CSV Sector 2 Fitness Samples
 */
std::vector<std::tuple<std::string,std::string,int>> Load_CSV_Fitness_Samples( const std::filesystem::path& coord_path );
/**
 * @file    Utilities.hpp
 * @author  Marvin Smith
 * @date    12/25/2020
 */
#pragma once

// C++ Libraries
#include <filesystem>
#include <vector>

// Project Libraries
#include "../src/Point.hpp"

/**
 * @brief Load the CSV Vertices
 */
std::vector<Point> Load_CSV_Vertices( const std::filesystem::path& coord_path,
                                      int                          epsg_code );
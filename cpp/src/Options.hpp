/**
 * @file    Options.hpp
 * @author  Marvin Smith
 * @date    12/19/2020
 */
#pragma once

// C++ Libraries
#include <filesystem>
#include <map>
#include <string>

// Project Libraries
#include "Exit_Condition.hpp"
#include "GA_Config.hpp"

/**
 * @class Options
 */
struct Options
{
    // Executable Name
    std::string program_name;

    // Database Name
    std::filesystem::path db_path;

    // Database Sector-ID
    int db_sector_id { -1 };

    // EPSG Code
    int epsg_code { 32613 };

    // Genetic Algorithm Config
    GA_Config ga_config;

    // Minimum Number of Waypoints
    int min_waypoints { 8 };

    // Max Number of Waypoints
    int max_waypoints { 14 };

    // Exit Condition
    Exit_Condition::ptr_t exit_condition;

    /// Population Size
    size_t population_size { 2000 };

    /// Max Number of Iterations
    size_t max_iterations { 500 };

    /// Exit Repeats ( Should be some percentage of the total iterations )
    size_t exit_repeats { (size_t)(0.1 * max_iterations) };

    double start_latitude { 0 };
    double start_longitude { 0 };
    double end_latitude { 0 };
    double end_longitude { 0 };

    // GA Fields
    double preservation_rate { 0.05 };
    double selection_rate { 0.4 };
    double mutation_rate { 0.8 };
    double random_vert_rate { 0.05 };

    // Application Overhead
    int number_threads = 10;

    // Flag if we want to load the population data rather than randomly generate
    bool load_population_data { false };

    //Path to the population file we'll write on close
    std::filesystem::path population_path { "./population.csv" };


}; // End of Options Class

/**
 * @brief Parse the Command-Line-Options
 */
Options Parse_Command_Line( int argc, char* argv[] );

/**
 * @brief Print Usage Instructions
 */
void Usage( const Options& options );
/**
 * @file    Options.hpp
 * @author  Marvin Smith
 * @date    12/19/2020
 */
#pragma once

// C++ Libraries
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
    std::string db_path;

    // EPSG Code
    int epsg_code { 32613 };

    // Genetic Algorithm Config
    GA_Config ga_config;

    // Minimum Number of Waypoints
    int min_waypoints { 4 };

    // Max Number of Waypoints
    int max_waypoints { 4 };

    // Exit Condition
    Exit_Condition::ptr_t exit_condition;

    /// Population Size
    size_t population_size { 50 };

    /// Max Number of Iterations
    size_t max_iterations { 10 };

    /// Exit Repeats ( Should be some percentage of the total iterations )
    size_t exit_repeats { (size_t)(0.1 * max_iterations) };

    // GA Fields
    double preservation_rate { 0.01 };
    double selection_rate { 0.5 };
    double mutation_rate { 0.7 };

}; // End of Options Class

/**
 * @brief Parse the Command-Line-Options
 */
Options Parse_Command_Line( int argc, char* argv[] );

/**
 * @brief Print Usage Instructions
 */
void Usage( const Options& options );
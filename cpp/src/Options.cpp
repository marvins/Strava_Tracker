/**
 * @file    Options.cpp
 * @author  Marvin Smith
 * @date    12/19/2020
 */
#include "Options.hpp"

// C++ Libraries
#include <deque>
#include <filesystem>
#include <iostream>

/************************************************/
/*          Parse Command-Line Options          */
/************************************************/
Options Parse_Command_Line( int argc, char* argv[] )
{
    Options output;
    output.program_name = argv[0];

    std::deque<std::string> args;
    for( int i=1; i<argc; i++ )
    {
        args.push_back( argv[i] );
    }

    // Parse Arguments
    while( !args.empty() )
    {
        // Grab next argument
        std::string arg = args.front();
        args.pop_front();

        // Check Help
        if( arg == "-h" || arg == "--help" )
        {
            Usage( output );
        }

        // Check DB Path
        if( arg == "-d" )
        {
            output.db_path = args.front();
            args.pop_front();
        }
    }

    // Check if filesystem exists
    if( !std::filesystem::exists( std::filesystem::path( output.db_path ) ) )
    {
        std::cerr << "database file does not exist: " << output.db_path << std::endl;
        Usage( output );
    }

    // Create Exit Condition
    output.exit_condition = std::make_shared<Exit_Condition>( output.exit_repeats, 0.001 );

    return output;
}

/****************************************************/
/*          Print the Usage Instructions            */
/****************************************************/
void Usage( const Options& options )
{
    std::cerr << "error: " << options.program_name << " -d <db-name> " << std::endl;
    std::cerr << std::endl;
    std::cerr << "   -h : Print usage instructions." << std::endl;
    std::cerr << std::endl;
    std::exit(-1);
}
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

// Boost Libraries
#include <boost/log/trivial.hpp>

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

        // Check Mutation Rate
        if( arg == "-m" )
        {
            output.mutation_rate = std::stod( args.front() );
            args.pop_front();
        }

        // Check Preservation Rate
        if( arg == "-p" )
        {
            output.preservation_rate = std::stod( args.front() );
            args.pop_front();
        }

        // Check Selection Rate
        if( arg == "-s" )
        {
            output.selection_rate = std::stod( args.front() );
            args.pop_front();
        }

        // Random Vert Rate
        if( arg == "-r" )
        {
            output.random_vert_rate = std::stod( args.front() );
            args.pop_front();
        }

        // Vertices
        if( arg == "-maxw" )
        {
            output.max_waypoints = std::stoi( args.front() );
            args.pop_front();
        }
        if( arg == "-minw" )
        {
            output.min_waypoints = std::stoi( args.front() );
            args.pop_front();
        }

        if( arg == "-err" )
        {
            output.exit_repeats = std::stod( args.front() );
            args.pop_front();
        }

        if( arg == "-epsg" )
        {
            output.epsg_code = std::stoi( args.front() );
            args.pop_front();
        }

        if( arg == "-pop" )
        {
            output.population_size = std::stoi( args.front() );
            args.pop_front();
        }
        if( arg == "-i" )
        {
            output.max_iterations = std::stoi( args.front() );
            args.pop_front();
        }
        if( arg == "-stats" )
        {
            output.ga_config.stats_output_pathname = args.front();
            args.pop_front();
        }
        if( arg == "-sector_id" )
        {
            output.db_sector_id = std::stoi( args.front() );
            args.pop_front();
        }
        if( arg == "-nt" )
        {
            output.number_threads = std::stoi( args.front() );
            args.pop_front();
        }
        if( arg == "-start_point" )
        {
            output.start_latitude = std::stod( args.front() );
            args.pop_front();
            output.start_longitude = std::stod( args.front() );
            args.pop_front();
        }
        if( arg == "-end_point" )
        {
            output.end_latitude = std::stod( args.front() );
            args.pop_front();
            output.end_longitude = std::stod( args.front() );
            args.pop_front();
        }
        if( arg == "-input" )
        {
            output.load_population_data = true;
            output.population_path = std::filesystem::path( args.front());
            args.pop_front();
        }
        if( arg == "-seed_id" )
        {
            output.seed_dataset_id = std::stoi( args.front() );
            args.pop_front();
        }
    }   

    // Check if filesystem exists
    if( !std::filesystem::exists( std::filesystem::path( output.db_path ) ) )
    {
        std::cerr << "database file does not exist: " << output.db_path << std::endl;
        Usage( output );
    }

    // Max Vertices

    // Create Exit Condition
    output.exit_condition = std::make_shared<Exit_Condition>( output.exit_repeats, 0.001 );

    output.ga_config.mutation_rate     = output.mutation_rate;
    output.ga_config.preservation_rate = output.preservation_rate;
    output.ga_config.selection_rate    = output.selection_rate;
    output.ga_config.random_vert_rate  = output.random_vert_rate;
    output.ga_config.number_threads    = output.number_threads;

    return output;
}

/****************************************************/
/*          Print the Usage Instructions            */
/****************************************************/
void Usage( const Options& options )
{
    std::stringstream sin;
    sin << "error: " << options.program_name << " -d <db-name> " << std::endl;
    sin << std::endl;
    sin << "   -h : Print usage instructions." << std::endl;
    sin << std::endl;
    sin << "Required Arguments" << std::endl;
    sin << "   -d <path> : Path to point database file." << std::endl;
    sin << "   -start_point <lat> <lon> : Starting coordinate" << std::endl;
    sin << "   -end_point <lat> <lon> : Ending coordinate" << std::endl;
    sin << "Optional Arguments" << std::endl;
    sin << "   -m <float> : Mutation Rate [0-1]" << std::endl;
    sin << "       - Default: " << options.mutation_rate << std::endl;
    sin << "   -p <float> : Preservation Rate [0-1]" << std::endl;
    sin << "       - Default: " << options.preservation_rate << std::endl;
    sin << "   -s <float> : Selection Rate [0-1]" << std::endl;
    sin << "       - Default: " << options.selection_rate << std::endl;
    sin << "   -r <float> : Random Vertex Mutation Rate [0-1]" << std::endl;
    sin << "       - Default: " << options.random_vert_rate << std::endl;
    sin << "   -maxw <int> : Max Number of Waypoints [1+]" << std::endl;
    sin << "       - Default: " << options.max_waypoints << std::endl;
    sin << "   -minw <int> : Min Number of Waypoints [1+]" << std::endl;
    sin << "       - Default: " << options.min_waypoints << std::endl;
    sin << "   -err <float> : Exit Repeat Ratio [0-1]" << std::endl;
    sin << "       - Default: " << options.exit_repeats << std::endl;
    sin << "   -epsg <int>  : EPSG Code for UTM Conversions" << std::endl;
    sin << "       - Default: " << options.epsg_code << std::endl;
    sin << "   -pop <int>   : Population Size [10+]" << std::endl;
    sin << "       - Default: " << options.population_size << std::endl;
    sin << "   -i <int>     : Max Number of Iterations for Population [1+]" << std::endl;
    sin << "       - Default: " << options.max_iterations << std::endl;
    sin << "   -stats <path>: Path to statistics file" << std::endl;
    sin << "       - Default: " << options.ga_config.stats_output_pathname << std::endl;
    sin << "   -sector_id <int>: Sector ID to query on.  Skip entry to do all points or value < 0" << std::endl;
    sin << "       - Default: " << options.db_sector_id << std::endl;
    sin << "   -nt <int>    : Number of threads to use in the population fitness update." << std::endl;
    sin << "       - Default: " << options.number_threads << std::endl;
    sin << "   -input <path> : Load the initial population data from disk." << std::endl;
    sin << "       - Default behavior is to randomly generate a population." << std::endl;
    sin << "         Too few entries will result in the remaining entries being randomly created." << std::endl;
    sin << "   -seed_id <int> : Initial dataset-id to use for seeding the initial population." << std::endl;
    sin << "                    If id < 0, then random numbers shall be used.  Also, using an input path will override this." << std::endl;
    sin << "       - Default: " << options.seed_dataset_id << std::endl;
    sin << std::endl;
    BOOST_LOG_TRIVIAL(warning) << sin.str();
    std::exit(-1);
}
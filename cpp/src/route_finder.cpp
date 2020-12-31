/**
 * @file   route_finder.cpp
 * @name   Marvin Smith
 * @date   12/19/2020
 */

// Project Libraries
#include "DB_Utils.hpp"
#include "GDAL_Utilities.hpp"
#include "Options.hpp"
#include "Sector_Runner.hpp"

// Boost Libraries
#include <boost/log/trivial.hpp>

using namespace std::placeholders;

int main( int argc, char* argv[] )
{
    // Overhead
    srand(time(nullptr));

    // Check Command-Line Arguments
    auto options = Parse_Command_Line( argc, argv );
    
    // Load the database
    auto db = Open_Database( options.db_path );

    // Load the list of sectors
    auto sector_ids = Load_Sector_Data( db );
   
    // Define our mutation and crossover algorithms
    WaypointList::crossover_func_tp crossover_algorithm = WaypointList::Crossover;
    WaypointList::mutation_func_tp  mutation_algorithm  = WaypointList::Mutation;
    WaypointList::random_func_tp    random_algorithm    = WaypointList::Randomize;

    // Build Coordinate Transformer
    auto xform_utm2dd = Create_UTM_to_DD_Transformation( options.epsg_code );
    auto xform_dd2utm = Create_DD_to_UTM_Transformation( options.epsg_code );

    // Global Stats Aggregator
    Stats_Aggregator stats_aggregator( options.ga_config.stats_output_pathname );
    stats_aggregator.Start_Writer();

    // Master List of Vertices
    Write_Worker::VTX_LIST_TP master_vertex_list;

    std::vector<Sector_Runner::ptr_t> runners;
    std::vector<std::thread> run_threads;

    size_t counter = 0;
    // Iterate over each sector
    for( const auto& sector_id : sector_ids )
    {
        runners.push_back( std::make_shared<Sector_Runner>( db, 
                                                            sector_id.first,
                                                            sector_id.second,
                                                            options,
                                                            xform_utm2dd,
                                                            xform_dd2utm,
                                                            master_vertex_list,
                                                            crossover_algorithm,
                                                            mutation_algorithm,
                                                            random_algorithm,
                                                            stats_aggregator ) );
        run_threads.emplace_back( &Sector_Runner::Run, runners[counter].get() );
        counter++;
    } // Let the destructor finish

    // Wait for all work to complete
    for( auto& runner : runners )
    {
        runner->Wait_Completion();
    }

    for( auto& thread : run_threads )
    {
        if( thread.joinable() )
        {
            thread.join();
        }
    }
    BOOST_LOG_TRIVIAL(debug) << "All Tasks Finished";

    // Cleanup
    sqlite3_close(db);

    return 0;
}

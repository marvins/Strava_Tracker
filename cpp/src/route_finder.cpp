/**
 * @file   route_finder.cpp
 * @name   Marvin Smith
 * @date   12/19/2020
 */

// Project Libraries
#include "Context.hpp"
#include "DB_Utils.hpp"
#include "GDAL_Utilities.hpp"
#include "Genetic_Algorithm.hpp"
#include "KML_Writer.hpp"
#include "Options.hpp"
#include "WaypointList.hpp"
#include "Write_Worker.hpp"

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
    sqlite3 *db;
    auto rc = sqlite3_open( options.db_path.c_str(), &db );

    // Make sure database is open
    if( rc )
    {
        std::cerr << "Can't open the database. Why: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }
    else
    {
      std::cerr << "Opened the database successfully" << std::endl;
    }

    // Load the list of sectors
    auto sector_ids = Load_Sector_IDs( db );
   
    // For Each Sector, Load the points
    auto point_list = Load_Point_List( db, options.db_sector_id );

    // Get point range
    auto point_range = Normalize_Points( point_list );
    size_t x_digits = log10(std::get<2>(point_range) - std::get<0>(point_range)) + 1;
    size_t y_digits = log10(std::get<3>(point_range) - std::get<1>(point_range)) + 1;
    size_t max_x = std::get<2>(point_range) - std::get<0>(point_range) + 1;
    size_t max_y = std::get<3>(point_range) - std::get<1>(point_range) + 1;
    BOOST_LOG_TRIVIAL(debug) << "X Digits: " << x_digits << ", Y Digits: " << y_digits;
    BOOST_LOG_TRIVIAL(debug) << "Min: [" << std::get<0>(point_range) << ", " << std::get<1>(point_range) 
                             << "], Max: [" << std::get<2>(point_range) << ", " << std::get<3>(point_range) << "]"; 

    // Define our mutation and crossover algorithms
    WaypointList::crossover_func_tp crossover_algorithm = WaypointList::Crossover;
    WaypointList::mutation_func_tp  mutation_algorithm  = WaypointList::Mutation;
    WaypointList::random_func_tp    random_algorithm    = WaypointList::Randomize;

    // Build Coordinate Transformer
    auto xform_utm2dd = Create_UTM_to_DD_Transformation( options.epsg_code );
    auto xform_dd2utm = Create_DD_to_UTM_Transformation( options.epsg_code );

    // Convert Start and End point to the normalized UTM
    auto start_point = Convert_Coordinate( xform_dd2utm, 
                                           ToPoint2D( options.start_latitude, 
                                                      options.start_longitude ) ) - ToPoint2D( std::get<0>(point_range),
                                                                                               std::get<1>(point_range) );
    auto end_point = Convert_Coordinate( xform_dd2utm, 
                                         ToPoint2D( options.end_latitude, 
                                                    options.end_longitude ) ) 
                     - ToPoint2D( std::get<0>(point_range), std::get<1>(point_range) );
    BOOST_LOG_TRIVIAL(debug) << "Starting Point: " << start_point.To_String() << ", Ending Point: " << end_point.To_String();
    
    // Create Quad Tree
    Rect point_bbox( ToPoint2D( -10, -10 ),
                     std::get<2>(point_range) - std::get<0>(point_range) + 20,
                     std::get<3>(point_range) - std::get<1>(point_range) + 20);
    int max_objects = 8;
    int max_levels = 10;

    // Construct the Context info
    Context context;
    context.point_list = point_list;
    context.density_step_distance = 25;
    context.start_point = start_point;
    context.end_point   = end_point;
    context.point_quad_tree = QuadTree<QTNode>( point_bbox, max_objects, max_levels );
    for( const auto& pt : context.point_list )
    {
        context.geo_point_list.push_back( ToPoint2D( pt.x_norm, pt.y_norm ) );
        auto node = std::make_shared<QTNode>( pt.index, context.geo_point_list.back() );
        context.point_quad_tree.Insert( node );
    }
    BOOST_LOG_TRIVIAL(debug) << "Context Created. Step-Distance: " << context.density_step_distance;
    auto context_ptr = reinterpret_cast<void*>( &context );


    // Master List of Vertices
    std::map<int,std::vector<DB_Point>> master_vertex_list;

    // Input population data (if requested)
    std::map<int,std::vector<WaypointList>> loaded_population;
    if( options.load_population_data )
    {
        loaded_population = Load_Population( options.population_path,
                                             options.min_waypoints,
                                             options.max_waypoints,
                                             options.population_size );
    }
    else if( options.seed_dataset_id >= 0 )
    {
        auto dataset_points = Load_Point_List( db, options.db_sector_id, options.seed_dataset_id );

        // Normalize
        Normalize_Points( dataset_points, 
                          std::get<0>(point_range),
                          std::get<1>(point_range) );
        std::vector<Point> dpoints;
        for( const auto& pt : dataset_points )
        {
            dpoints.push_back( ToPoint2D( pt.x_norm, pt.y_norm ) );
        }
        loaded_population = Seed_Population( dpoints,
                                             options.min_waypoints,
                                             options.max_waypoints,
                                             options.population_size,
                                             max_x, max_y, 
                                             start_point,
                                             end_point );
    }

    // Create the file writing information
    auto writer_obj = std::make_shared<Write_Worker>( xform_utm2dd,
                                                      point_range,
                                                      point_list.front().gz,
                                                      master_vertex_list );
    std::function<void(const WaypointList&, size_t)> write_worker = std::bind( &Write_Worker::Write, writer_obj, _1, _2 );

    // Iterate over each waypoint count
    for( int num_waypoints = options.min_waypoints; 
         num_waypoints <= options.max_waypoints;
         num_waypoints++ )
    {
        // Build the initial population
        std::vector<WaypointList> initial_population;
        if( !loaded_population.empty() )
        {
            initial_population = Build_Random_Waypoints( options.population_size,
                                                         num_waypoints,
                                                         max_x, max_y,
                                                         start_point,
                                                         end_point );
        }
        else
        {
            initial_population = loaded_population[num_waypoints];
        }
        
        // Load the population list
        BOOST_LOG_TRIVIAL(debug) << "Initial Population List, " << Print_Population_List( initial_population, 10 );

        // Construct Genetic Algorithm
        Genetic_Algorithm<WaypointList> ga( options.ga_config,
                                            initial_population,
                                            crossover_algorithm,
                                            mutation_algorithm,
                                            random_algorithm,
                                            write_worker,
                                            (num_waypoints != options.min_waypoints) );

        // Run the GA
        auto population = ga.Run( options.max_iterations,
                                  options.exit_condition,
                                  context_ptr );

        // Check our results
        BOOST_LOG_TRIVIAL(debug) << "Most Fit Population List, " << Print_Population_List( population, 10 );
        BOOST_LOG_TRIVIAL(debug) << "Best Fit Item: " << population[0].To_String(true);

        write_worker( population[0], options.max_iterations );  

        // Write the population data to disk
        Write_Population( population, options.population_path, true );
    
    } // End of Waypoint Number Loop

    // Cleanup
    sqlite3_close(db);

    return 0;
}

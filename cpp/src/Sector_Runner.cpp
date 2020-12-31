/**
 * @file    Sector_Runner.cpp
 * @author  Marvin Smith
 * @date    12/30/2020
 */
#include "Sector_Runner.hpp"

// Boost Libraries
#include <boost/log/trivial.hpp>

// C++ Libraries
#include <functional>

// Project Libraries
#include "Context.hpp"
#include "Genetic_Algorithm.hpp"
#include "WaypointList.hpp"
#include "Write_Worker.hpp"

using namespace std::placeholders;

/********************************/
/*          Constructor         */
/********************************/
Sector_Runner::Sector_Runner( sqlite3*                             db,
                              const std::string&                   sector_id,
                              const std::tuple<DB_Point,DB_Point>& sector_endpoints,
                              const Options&                       options,
                              OGRCoordinateTransformation*         xform_utm2dd,
                              OGRCoordinateTransformation*         xform_dd2utm,
                              Write_Worker::VTX_LIST_TP&           master_vertex_list,
                              WaypointList::crossover_func_tp      crossover_algorithm,
                              WaypointList::mutation_func_tp       mutation_algorithm,
                              WaypointList::random_func_tp         random_algorithm,
                              Stats_Aggregator&                    stats_aggregator )
  : m_db( db ),
    m_sector_id( sector_id ),
    m_sector_endpoints( sector_endpoints ),
    m_options( options ),
    m_xform_utm2dd( xform_utm2dd ),
    m_xform_dd2utm( xform_dd2utm ),
    m_master_vertex_list( master_vertex_list ),
    m_crossover_algorithm( crossover_algorithm ),
    m_mutation_algorithm( mutation_algorithm ),
    m_random_algorithm( random_algorithm ),
    m_stats_aggregator( stats_aggregator )
{
    BOOST_LOG_TRIVIAL(debug) << "Constructed Runner for Sector: " << m_sector_id;
}

/****************************/
/*          Runner          */
/****************************/
void Sector_Runner::Run()
{
    BOOST_LOG_TRIVIAL(debug) << "Start of Runner for Sector: " << m_sector_id;
    std::lock_guard<std::mutex> lck(m_run_mtx);

    try
    {
        // For the sector, load the points
        auto point_list = Load_Point_List( m_db, m_sector_id );

        // Get point range
        auto point_range = Normalize_Points( point_list );
        size_t x_digits = log10(std::get<2>(point_range) - std::get<0>(point_range)) + 1;
        size_t y_digits = log10(std::get<3>(point_range) - std::get<1>(point_range)) + 1;
        size_t max_x = std::get<2>(point_range) - std::get<0>(point_range) + 1;
        size_t max_y = std::get<3>(point_range) - std::get<1>(point_range) + 1;
        BOOST_LOG_TRIVIAL(debug) << "Sector: " << m_sector_id << ", X Digits: " << x_digits << ", Y Digits: " << y_digits;
        BOOST_LOG_TRIVIAL(debug) << "Sector: " << m_sector_id << ", Min: [" << std::get<0>(point_range) << ", " << std::get<1>(point_range) 
                                 << "], Max: [" << std::get<2>(point_range) << ", " << std::get<3>(point_range) << "]"; 

        // Convert Start and End point to the normalized UTM
        auto start_point = Convert_Coordinate( m_xform_dd2utm, 
                                               std::get<0>( m_sector_endpoints ).Get_LLA_Coordinate() ) - ToPoint2D( std::get<0>(point_range),
                                                                                                                     std::get<1>(point_range) );
        auto end_point = Convert_Coordinate( m_xform_dd2utm, 
                                             std::get<1>( m_sector_endpoints ).Get_LLA_Coordinate() ) - ToPoint2D( std::get<0>(point_range), 
                                                                                                                   std::get<1>(point_range) );
        BOOST_LOG_TRIVIAL(debug) << "Sector: " << m_sector_id << ", Starting Point: " << start_point.To_String() << ", Ending Point: " << end_point.To_String();

        // Construct the Context info
        Context context;
        context.point_list = point_list;
        context.start_point = start_point;
        context.end_point   = end_point;
        for( const auto& pt : context.point_list )
        {
            context.geo_point_list.push_back( ToPoint2D( pt.x_norm, pt.y_norm ) );
        }
        auto context_ptr = reinterpret_cast<void*>( &context );

        // Input population data (if requested)
        std::map<int,std::vector<WaypointList>> loaded_population;
        if( m_options.load_population_data )
        {
            loaded_population = Load_Population( m_options.population_path,
                                                 m_options.min_waypoints,
                                                 m_options.max_waypoints,
                                                 m_options.population_size );
        }
        else if( m_options.seed_dataset_id >= 0 )
        {
            auto dataset_points = Load_Point_List( m_db, 
                                                   m_sector_id,
                                                   m_options.seed_dataset_id );

            // Normalize
            Normalize_Points( dataset_points, 
                              std::get<0>(point_range),
                              std::get<1>(point_range) );
            std::vector<Point> dpoints;
            for( const auto& pt : dataset_points )
            {
                dpoints.push_back( ToPoint2D( pt.x_norm, pt.y_norm ) );
            }

            BOOST_LOG_TRIVIAL(debug) << "Sector: " << m_sector_id << ", Building Population from Dataset " << m_options.seed_dataset_id;
            loaded_population = Seed_Population( dpoints,
                                                 m_options.min_waypoints,
                                                 m_options.max_waypoints,
                                                 m_options.population_size,
                                                 max_x, max_y, 
                                                 start_point,
                                                 end_point );
        }

        // Create the file writing information
        auto writer_obj = std::make_shared<Write_Worker>( m_xform_utm2dd,
                                                          point_range,
                                                          point_list.front().gz,
                                                          m_master_vertex_list );
        Write_Worker::writer_func_tp write_worker = std::bind( &Write_Worker::Write, writer_obj, _1, _2, _3 );

        // Iterate over each waypoint count
        for( int num_waypoints = m_options.min_waypoints; 
             num_waypoints <= m_options.max_waypoints;
             num_waypoints++ )
        {
            // Build the initial population
            std::vector<WaypointList> initial_population;
            if( loaded_population.empty() )
            {
                initial_population = Build_Random_Waypoints( m_options.population_size,
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
            BOOST_LOG_TRIVIAL(debug) << "Sector: " << m_sector_id << ", Initial Population List, " << Print_Population_List( initial_population, 10 );

            // Construct Genetic Algorithm
            Genetic_Algorithm<WaypointList> ga( m_options.ga_config,
                                                initial_population,
                                                m_crossover_algorithm,
                                                m_mutation_algorithm,
                                                m_random_algorithm,
                                                write_worker,
                                                m_stats_aggregator );

            // Run the GA
            auto population = ga.Run( m_sector_id,
                                      m_options.max_iterations,
                                      m_options.exit_condition,
                                      context_ptr );

            // Check our results
            BOOST_LOG_TRIVIAL(debug) << "Sector: " << m_sector_id << ", Most Fit Population List, " << Print_Population_List( population, 10 );
            BOOST_LOG_TRIVIAL(debug) << "Sector: " << m_sector_id << ", Best Fit Item: " << population[0].To_String(true);

            // Write the population data to disk
            Write_Population( population, 
                              m_sector_id,
                              m_options.population_path, 
                              true );
    
        } // End of Waypoint Number Loop
    } 
    catch( std::exception& e )
    {
        BOOST_LOG_TRIVIAL(error) << "Sector: " << m_sector_id << ", Caught Exception: " << e.what();
        std::exit(1);
    }
}

/****************************************************/
/*          Block Until Completion Finished         */
/****************************************************/
void Sector_Runner::Wait_Completion()
{
    std::lock_guard<std::mutex> lck(m_run_mtx);
}
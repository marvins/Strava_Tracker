/**
 * @file   route_finder.cpp
 * @name   Marvin Smith
 * @date   12/19/2020
 */

// Project Libraries
#include "DB_Utils.hpp"
#include "GDAL_Utilities.hpp"
#include "Genetic_Algorithm.hpp"
#include "KML_Writer.hpp"
#include "Options.hpp"
#include "WaypointList.hpp"

// C++ Libraries
#include <cmath>
#include <iostream>
#include <fstream>

// Boost Libraries
#include <boost/log/trivial.hpp>

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
    auto point_list = Load_Point_List( db );

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

    auto context = reinterpret_cast<void*>( &point_list );

    // Build Coordinate Transformer
    auto coord_xform = Create_UTM_to_DD_Transformation( options.epsg_code );

    // Master List of Vertices
    std::map<int,std::vector<DB_Point>> master_vertex_list;

    // /////////////////////////////////////////////
    // /////////////////////////////////////////////
    // auto wp1 = WaypointList( "14714340940862256040040220767130287", 5, max_x, max_y );
    // auto wp2 = WaypointList( "40220761471434094086225604007130287", 5, max_x, max_y );
    // wp1.Update_Fitness( context );
    // wp2.Update_Fitness( context );
    // std::cout << wp1.To_String(true) << std::endl;
    // std::cout << wp2.To_String(true) << std::endl;
    // return 0;
    // //////////////////////////////////////////////
    // //////////////////////////////////////////////

    // Iterate over each waypoint count
    for( int num_waypoints = options.min_waypoints; 
         num_waypoints <= options.max_waypoints;
         num_waypoints++ )
    {
        // Build the initial population
        std::vector<WaypointList> initial_population = Build_Random_Waypoints( options.population_size,
                                                                               num_waypoints,
                                                                               max_x, max_y );
        
        BOOST_LOG_TRIVIAL(debug) << "Initial Population List, " << Print_Population_List( initial_population, 10 );

        // Construct Genetic Algorithm
        Genetic_Algorithm<WaypointList> ga( options.ga_config,
                                            initial_population,
                                            crossover_algorithm,
                                            mutation_algorithm,
                                            random_algorithm );

        // Run the GA
        auto population = ga.Run( options.max_iterations,
                                  options.exit_condition,
                                  context );

        // Check our results
        BOOST_LOG_TRIVIAL(debug) << "Most Fit Population List, " << Print_Population_List( population, 10 );

        // Store the results of this run
        std::vector<DB_Point> vertex_point_list;
        auto vertices = population.front().Get_Vertices();
        for( auto& v : vertices )
        {
            DB_Point new_point;

            // Add the UTM offsets
            v += Point<double>( std::get<0>(point_range), std::get<1>(point_range) );
            new_point.gz       = point_list.front().gz;
            new_point.easting  = v.x();
            new_point.northing = v.y();

            new_point.x_norm = population.front().Get_Fitness();

            // Convert to Geographic
            auto temp_lla = Convert_Coordinate( coord_xform, v );
            new_point.latitude  = temp_lla.m_data[0];
            new_point.longitude = temp_lla.m_data[1];
            vertex_point_list.push_back( new_point );
        }
        master_vertex_list[num_waypoints] = vertex_point_list;

    } // End of Waypoint Number Loop

    std::ofstream fout;
    fout.open("waypoints.csv");
    fout << "NumWaypoints,Fitness,GridZone,Easting,Northing,Latitude,Longitude" << std::endl;
    for( const auto& num : master_vertex_list )
    {
        std::cerr << "Writing vertices for " << num.first << " points" << std::endl;
        for( const auto& point : num.second )
        {
            fout << std::fixed << num.first << "," << point.x_norm << "," 
                 << point.gz << "," << point.easting << "," << point.northing 
                 << "," << point.latitude << "," << point.longitude << std::endl;
        }
    }
    fout.close();

    Write_KML( "waypoints.kml", master_vertex_list );    

    // Cleanup
    sqlite3_close(db);

    return 0;
}

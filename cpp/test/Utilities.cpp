/**
 * @file    Utilities.cpp
 * @author  Marvin Smith
 * @date    12/25/2020
 */
#include "Utilities.hpp"

// Boost Libraries
#include <boost/algorithm/string.hpp>

// C++ Libraries
#include <fstream>

// Project Libraries
#include "../src/GDAL_Utilities.hpp"

/****************************************/
/*          Load the CSV File           */
/****************************************/
std::vector<Point> Load_CSV_Vertices( const std::filesystem::path& coord_path,
                                      int                          epsg_code )
{
    std::vector<Point> vertices_lla;
    std::ifstream fin;
    fin.open( coord_path.c_str() );
    std::string line;
    std::getline( fin, line ); // skip first line (header)
    
    while( !fin.eof() )
    {
        std::getline( fin, line );

        // Parse entry
        boost::trim( line );
        if( !line.empty() )
        {
            // Split by comma
            std::vector<std::string> parts;
            boost::split( parts, line, boost::is_any_of(","));
            vertices_lla.push_back( ToPoint2D( std::stod( parts[1] ), 
                                               std::stod( parts[0] ) ) );
        }
    }
    fin.close();

    // Convert LLA Coordinates to UTM
    std::vector<Point> vertices_utm;
    auto xform_dd2utm = Create_DD_to_UTM_Transformation( epsg_code );
    for( const auto& v : vertices_lla )
    {
        vertices_utm.push_back( Convert_Coordinate( xform_dd2utm, v ) );
    }

    return vertices_utm;
}

/****************************************************/
/*          Load the CSV Sector Fitness Samples     */
/****************************************************/
std::vector<std::tuple<std::string,std::string,int>> Load_CSV_Fitness_Samples( const std::filesystem::path& coord_path )
{
    std::vector<std::tuple<std::string,std::string,int>> output;
    
    std::ifstream fin;
    fin.open( coord_path.c_str() );
    std::string line;
    std::getline( fin, line ); // skip first line (header)
    while( !fin.eof() )
    {
        std::getline( fin, line );

        // Parse entry
        boost::trim( line );
        if( !line.empty() )
        {
            // Split by comma
            std::vector<std::string> parts;
            boost::split( parts, line, boost::is_any_of(","));

            int num_waypoints = std::stoi( parts[2] );
            std::string name  = parts[1];
            std::string dna   = parts[0];
            output.push_back( std::make_tuple( name, dna, num_waypoints ));
        }
    }
    fin.close();

    return output;
}
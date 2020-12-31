/**
 * @file    Write_Worker.cpp
 * @author  Marvin Smith
 * @date    12/24/2020
*/
#include "Write_Worker.hpp"

// Project Libraries
#include "DB_Point.hpp"
#include "KML_Writer.hpp"

// C++ Libraries
#include <filesystem>
#include <fstream>
#include <iostream>

// Boost Libraries
#include <boost/log/trivial.hpp>

/********************************/
/*          Constructor         */
/********************************/
Write_Worker::Write_Worker( OGRCoordinateTransformation*             xform_utm2dd,
                            std::tuple<double,double,double,double>  point_range,
                            int                                      utm_gz,
                            VTX_LIST_TP&                             master_vertex_list )
  : m_xform_utm2dd( xform_utm2dd ),
    m_point_range( std::move( point_range ) ),
    m_utm_gz( utm_gz ),
    m_master_vertex_list(master_vertex_list)
{
}

/****************************************/
/*          Write data to disk          */
/****************************************/
void Write_Worker::Write( const WaypointList& wp,
                          const std::string&  sector_id,
                          size_t              iteration )
{
    // Store the results of this run
    std::vector<DB_Point> vertex_point_list;
    auto vertices = wp.Get_Vertices();
    for( auto& v : vertices )
    {
        DB_Point new_point;
        new_point.datasetId = wp.Get_DNA();
        new_point.index     = iteration;

        // Add the UTM offsets
        v += ToPoint2D( std::get<0>(m_point_range), std::get<1>(m_point_range) );
        new_point.gz       = m_utm_gz;
        new_point.easting  = v.x();
        new_point.northing = v.y();

        new_point.x_norm = wp.Get_Fitness();

        // Convert to Geographic
        auto temp_lla = Convert_Coordinate( m_xform_utm2dd, v );
        new_point.latitude  = temp_lla.m_data[0];
        new_point.longitude = temp_lla.m_data[1];
        vertex_point_list.push_back( new_point );
    }
    m_master_vertex_list[sector_id][wp.Get_Number_Waypoint()][iteration] = vertex_point_list;


    // Write Latest Results
    std::filesystem::path pname( "./waypoints.csv" );
    BOOST_LOG_TRIVIAL(debug) << "Sector: " << sector_id << ", Iteration: " << iteration << ", Writing Waypoint Data: " << pname.string();
    std::ofstream fout;
    fout.open( pname.string() );
    fout << "SectorId,NumWaypoints,Iteration,Fitness,GridZone,Easting,Northing,Latitude,Longitude,DNA" << std::endl;

    // Loop over Sectors
    for( const auto& sec : m_master_vertex_list )
    {
        // Loop over Waypoints
        for( const auto& num : sec.second )
        {
            // Loop over iterations
            for( const auto& iter : num.second )
            {
                // Loop over points
                for( const auto& point : iter.second )
                {
                    fout << sec.first << "," << num.first << std::fixed << "," << iter.first << "," << point.x_norm << "," 
                         << point.gz << "," << point.easting << "," << point.northing 
                         << "," << point.latitude << "," << point.longitude << ","
                         << point.datasetId << std::endl;
                } // End of Point Loop
            } // End of Iteration Loop
        } // End of Waypoint Loop
    } // End of Sector Loop
    fout.close();

    std::string waypoint_pathname = "waypoints.kml";
    BOOST_LOG_TRIVIAL(debug) << "Writing KML data to " << waypoint_pathname;
    Write_KML( waypoint_pathname.c_str(), m_master_vertex_list );  
}
/**
 * @file    Point.cpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include "DB_Point.hpp"

// C++ Libraries
#include <iostream>
#include <sstream>
#include <vector>

/********************************************/
/*          Get the LLA Coordinate          */
/********************************************/
Point DB_Point::Get_LLA_Coordinate() const
{
    return ToPoint2D( latitude, longitude );
}

/************************************/
/*          Write To String         */
/************************************/
std::string DB_Point::To_String() const
{
    std::stringstream sout;
    sout << "Point.  index: " << index << std::endl;
    sout << "  - Latitude: " << std::fixed << latitude << std::endl;
    sout << "  - Longitude: " << std::fixed << longitude << std::endl;
    sout << "  - GridZone: " << gz << std::endl;
    sout << "  - Easting: " << easting << std::endl;
    sout << "  - Northing: " << northing << std::endl;
    sout << "  - SectorID: " << sectorId << std::endl;
    sout << "  - DatasetID: " << datasetId << std::endl;
    sout << "  - X Norm: " << x_norm << std::endl;
    sout << "  - Y Norm: " << y_norm << std::endl;
    return sout.str();
}

/************************************************/
/*          Normalize the Point Range           */
/************************************************/
std::tuple<int,int,int,int> Normalize_Points( std::vector<DB_Point>& point_list, 
                                              int                    min_x,
                                              int                    min_y )
{
    // Compute min and max of the easting and northings
    auto range = std::make_tuple( (size_t)point_list.front().easting,
                                  (size_t)point_list.front().northing,
                                  (size_t)point_list.front().easting,
                                  (size_t)point_list.front().northing );

    for( const auto& p : point_list )
    {
        std::get<0>(range) = std::min( std::get<0>(range), (size_t)p.easting );
        std::get<1>(range) = std::min( std::get<1>(range), (size_t)p.northing );
        std::get<2>(range) = std::max( std::get<2>(range), (size_t)p.easting );
        std::get<3>(range) = std::max( std::get<3>(range), (size_t)p.northing );
    }

    if( min_x >= 0 )
    {
        std::get<0>(range) = min_x;
    }
    if( min_y >= 0 )
    {
        std::get<1>(range) = min_y;
    }

    // Compute the normalized values
    for( auto& p : point_list )
    {
        p.x_norm = p.easting  - std::get<0>(range);
        p.y_norm = p.northing - std::get<1>(range);
    }
    return range;
}
/**
 * @file    Write_Worker.hpp
 * @author  Marvin Smith
 * @date    12/24/2020
*/
#pragma once

// Project Libraries
#include "DB_Point.hpp"
#include "GDAL_Utilities.hpp"
#include "Stats_Aggregator.hpp"
#include "WaypointList.hpp"

// C++ Libraries
#include <map>
#include <tuple>
#include <vector>

/**
 * @brief Write Population data to disk.
*/
class Write_Worker
{
    public:

        /**
         * @brief Constructor
        */
        Write_Worker( OGRCoordinateTransformation*            xform_utm2dd,
                      std::tuple<double,double,double,double> point_range,
                      int                                     utm_gz,
                      std::map<int,std::vector<DB_Point>>&    master_vertex_list );

        /**
         * @brief Update the population data.
        */
        void Write( const WaypointList& wp,
                    size_t              iteration );

    private:

        /// Coordinate Transformer
        OGRCoordinateTransformation* m_xform_utm2dd;

        /// Point Information
        std::tuple<double,double,double,double> m_point_range;

        /// UTM Grid Zone
        int m_utm_gz;

        std::map<int,std::vector<DB_Point>>& m_master_vertex_list;

}; // End of Write_Worker Class
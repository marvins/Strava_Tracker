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
#include <functional>
#include <map>
#include <tuple>
#include <vector>

/**
 * @brief Write Population data to disk.
*/
class Write_Worker
{
    public:

        typedef std::function<void(const WaypointList&, const std::string&, size_t)> writer_func_tp;

        typedef std::map<std::string,std::map<int,std::map<int,std::vector<DB_Point>>>> VTX_LIST_TP;

        /**
         * @brief Constructor
        */
        Write_Worker( OGRCoordinateTransformation*             xform_utm2dd,
                      std::tuple<double,double,double,double>  point_range,
                      int                                      utm_gz,
                      VTX_LIST_TP&                             master_vertex_list );

        /**
         * @brief Update the population data.
        */
        void Write( const WaypointList& wp,
                    const std::string&  sector_id,
                    size_t              iteration );

    private:

        /// Coordinate Transformer
        OGRCoordinateTransformation* m_xform_utm2dd;

        /// Point Information
        std::tuple<double,double,double,double> m_point_range;

        /// UTM Grid Zone
        int m_utm_gz;

        // Sector-ID, WP, Iteration,DB-Point
        VTX_LIST_TP& m_master_vertex_list;

}; // End of Write_Worker Class
/**
 * @file    Sector_Runner.hpp
 * @author  Marvin Smith
 * @date    12/30/2020
 */
#pragma once

// Project Libraries
#include "DB_Utils.hpp"
#include "GDAL_Utilities.hpp"
#include "Options.hpp"
#include "Stats_Aggregator.hpp"
#include "Write_Worker.hpp"

// C++ Libraries
#include <memory>
#include <mutex>

/**
 * @class Sector_Runner
 */
class Sector_Runner
{
    public:

        /// Pointer Type
        typedef std::shared_ptr<Sector_Runner> ptr_t;

        /**
         * @brief Constructor
         */
        Sector_Runner( sqlite3*                             db,
                       const std::string&                   sector_id,
                       const std::tuple<DB_Point,DB_Point>& sector_endpoints,
                       const Options&                       options,
                       OGRCoordinateTransformation*         xform_utm2dd,
                       OGRCoordinateTransformation*         xform_dd2utm,
                       Write_Worker::VTX_LIST_TP&           master_vertex_list,
                       WaypointList::crossover_func_tp      crossover_algorithm,
                       WaypointList::mutation_func_tp       mutation_algorithm,
                       WaypointList::random_func_tp         random_algorithm,
                       Stats_Aggregator&                    stats_aggregator );

        /**
         * @brief Run the algorithm for the constructed Sector-ID
         */
        void Run();

        void Wait_Completion();

    private:

        /// Database Handle
        sqlite3* m_db;

        /// Sector Name
        std::string m_sector_id;

        /// Sector Data
        std::tuple<DB_Point,DB_Point> m_sector_endpoints;

        /// Options Class
        Options m_options;

        /// UTM to DD Converter
        OGRCoordinateTransformation*  m_xform_utm2dd;

        /// DD to UTM Converter
        OGRCoordinateTransformation*  m_xform_dd2utm;

        /// Master Vertex List
        Write_Worker::VTX_LIST_TP& m_master_vertex_list;

        /// Crossover Algorithm
        WaypointList::crossover_func_tp m_crossover_algorithm;

        /// Mutation Algorithm
        WaypointList::mutation_func_tp m_mutation_algorithm;

        /// Random Algorithm
        WaypointList::random_func_tp m_random_algorithm;

        /// Stats Aggregator
        Stats_Aggregator&  m_stats_aggregator;

        /// Run Mutex
        std::mutex m_run_mtx;

}; // End of Sector_Runner Class
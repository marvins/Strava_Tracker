/**
 * @file    Stats_Aggregator.hpp
 * @author  Marvin Smith
 * @date    12/21/2020
 */
#pragma once

// C++ Libraries
#include <chrono>
#include <map>
#include <mutex>
#include <string>
#include <tuple>

// Project Libraries
#include "Accumulator.hpp"

/**
 * @brief Simple utility class for storing useful metrics for post-processing
 */
class Stats_Aggregator
{
    public:

        /**
         * @brief Print Stats Information
         */
        virtual ~Stats_Aggregator();

        /**
         * @brief Report Timing Info
         */
        void Report_Timing( const std::string&  subsystem,
                            double              elapsed_time );

        /**
         * @brief Report end of a cycle.
         */
        void Report_Iteration_Complete( const std::string& sector_id,
                                        size_t             num_waypoints,
                                        size_t             iteration_number,
                                        double             best_fitness,
                                        double             iteration_time_ms );

        /**
         * @brief Report a duplicate entry.
         */
        void Report_Duplicate_Entry( const std::string& sector_id,
                                     size_t             num_waypoints,
                                     size_t             iteration_number );

        /**
         * @brief Writing Stats Data to File
         */
        void Write_Stats_Info( const std::string& output_pathname,
                               bool               append_file );

    private:

        /// Timing Information
        std::map<std::string,Accumulator<double>> m_timing_info;
        mutable std::mutex m_timing_mtx;

        /// Iteration Information [sector_id, NumWaypoints, Iteration, [Fitness/Accumulator]]
        std::map<std::string, std::map<size_t,std::map<size_t,std::tuple<double,double>>>> m_iteration_info;

        /// Duplicate Tracker
        std::map<std::string,std::map<size_t,std::map<size_t,size_t>>> m_duplicate_info;

}; // End of Stats_Aggregator Class
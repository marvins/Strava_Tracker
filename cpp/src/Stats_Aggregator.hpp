/**
 * @file    Stats_Aggregator.hpp
 * @author  Marvin Smith
 * @date    12/21/2020
 */
#pragma once

// C++ Libraries
#include <chrono>
#include <condition_variable>
#include <deque>
#include <map>
#include <mutex>
#include <string>
#include <thread>
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
         * @brief Constructor
         */
        Stats_Aggregator( const std::string& output_filename );

        /**
         * @brief Print Stats Information
         */
        virtual ~Stats_Aggregator();

        void Start_Writer();

        void Stop_Writer();

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
                                     size_t             iteration_number,
                                     size_t             number_duplicates );

    private:

        /**
         * @brief Writing Stats Data to File
         */
        void Write_Stats_Info();

        /// Output Pathname
        std::string m_output_pathname;

        /// Timing Information
        std::map<std::string,Accumulator<double>> m_timing_info;

        /// Iteration Information [sector_id, NumWaypoints, Iteration, [Fitness/Accumulator]]
        std::deque<std::string> m_iteration_info;

        /// Duplicate Tracker
        std::deque<std::string> m_duplicate_info;

        /// Access Lock
        mutable std::mutex m_timing_mtx;
        mutable std::mutex m_iter_mtx;
        mutable std::mutex m_dup_mtx;

        /// Write Thread
        std::thread m_write_thread;
        bool m_okay_to_run { true };

}; // End of Stats_Aggregator Class
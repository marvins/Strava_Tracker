/**
 * @file    Stats_Aggregator.cpp
 * @author  Marvin Smith
 * @date    12/21/2020
 */
#include "Stats_Aggregator.hpp"

// Boost Libraries
#include <boost/log/trivial.hpp>

// C++ Libraries
#include <fstream>
#include <iostream>
#include <sstream>

/********************************/
/*          Destructor          */
/********************************/
Stats_Aggregator::~Stats_Aggregator()
{
    for( const auto& subsystem : m_timing_info )
    {
        BOOST_LOG_TRIVIAL(info) << subsystem.second.To_String( "Subsystem: " + subsystem.first );
    }
}

/****************************************/
/*          Report Timing Info          */
/****************************************/
void Stats_Aggregator::Report_Timing( const std::string&               subsystem,
                                      const std::chrono::microseconds& elapsed_time )
{
    m_timing_info[subsystem].Insert( elapsed_time.count() );
}

/****************************************************/
/*          Report Iteration Information            */
/****************************************************/
void Stats_Aggregator::Report_Iteration_Complete( size_t   num_waypoints,
                                                  size_t   iteration_number,
                                                  double   best_fitness,
                                                  double   iteration_time_ms )
{
    m_iteration_info[num_waypoints][iteration_number] = std::make_tuple( best_fitness,
                                                                         iteration_time_ms );
}

/************************************************/
/*          Report a Duplicate Entry            */
/************************************************/
void Stats_Aggregator::Report_Duplicate_Entry( size_t   num_waypoints,
                                               size_t   iteration_number )
{
    m_duplicate_info[num_waypoints][iteration_number]++;
}

/****************************************/
/*          Write Stats Info            */
/****************************************/
void Stats_Aggregator::Write_Stats_Info( const std::string& output_pathname )
{
    // Write the iteration file
    std::ofstream fout;
    fout.open((output_pathname + ".iteration.csv").c_str());
    fout << "NumWaypoints,Iteration,BestFitness,IterationTimeMs" << std::endl;
    for( const auto& wp : m_iteration_info )
    {
        for( const auto& iteration : wp.second )
        {
            fout << wp.first << "," << iteration.first << "," << std::get<0>(iteration.second) << "," << std::get<1>(iteration.second) << std::endl;
        }
    }
    fout.close();

    // Write Duplicate File
    fout.open((output_pathname + ".duplicates.csv").c_str());
    fout << "NumWaypoints,Iteration,NumberDuplicates" << std::endl;
    for( const auto& wp : m_duplicate_info )
    {
        for( const auto& iteration : wp.second )
        {
            fout << wp.first << "," << iteration.first << "," << iteration.second << std::endl;
        }
    }
    fout.close();
}
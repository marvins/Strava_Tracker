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
        BOOST_LOG_TRIVIAL(info) << subsystem.second.To_String( "Subsystem: " + subsystem.first, "sec" );
    }
}

/****************************************/
/*          Report Timing Info          */
/****************************************/
void Stats_Aggregator::Report_Timing( const std::string&  subsystem,
                                      double              elapsed_time )
{
    m_timing_info[subsystem].Insert( elapsed_time );
}

/****************************************************/
/*          Report Iteration Information            */
/****************************************************/
void Stats_Aggregator::Report_Iteration_Complete( const std::string& sector_id,
                                                  size_t             num_waypoints,
                                                  size_t             iteration_number,
                                                  double             best_fitness,
                                                  double             iteration_time_ms )
{
    BOOST_LOG_TRIVIAL(debug) << "Logging Iteration Complete. Sector: " << sector_id
                             << ", Waypoints: " << num_waypoints << ", Iteration: " 
                             << iteration_number << ", Fitness: " << best_fitness 
                             << std::fixed << ", Time: " << iteration_time_ms;
    std::lock_guard<std::mutex> lck( m_timing_mtx );
    if( m_iteration_info.find(sector_id) == m_iteration_info.end() )
    {
        m_iteration_info[sector_id] = std::map<size_t,std::map<size_t,std::tuple<double,double>>>();
    }
    if( m_iteration_info[sector_id].find(num_waypoints) == m_iteration_info[sector_id].end() )
    {
        m_iteration_info[sector_id][num_waypoints] = std::map<size_t,std::tuple<double,double>>();
    }
    m_iteration_info[sector_id][num_waypoints][iteration_number] = std::make_tuple( best_fitness,
                                                                                    iteration_time_ms );
}

/************************************************/
/*          Report a Duplicate Entry            */
/************************************************/
void Stats_Aggregator::Report_Duplicate_Entry( const std::string& sector_id,
                                               size_t             num_waypoints,
                                               size_t             iteration_number )
{
    m_duplicate_info[sector_id][num_waypoints][iteration_number]++;
}

/****************************************/
/*          Write Stats Info            */
/****************************************/
void Stats_Aggregator::Write_Stats_Info( const std::string& output_pathname,
                                         bool               append_file )
{
    // Write the iteration file
    std::ofstream fout;

    auto pname = output_pathname + ".iteration.csv";
    BOOST_LOG_TRIVIAL(debug) << "Opening " << pname;
    fout.open(pname.c_str());
    fout << "SectorId,NumWaypoints,Iteration,BestFitness,IterationTimeSec" << std::endl;
    
    for( const auto& sec : m_iteration_info )
    {
        for( const auto& wp : sec.second )
        {
            for( const auto& iteration : wp.second )
            {
                fout << sec.first << "," << wp.first << "," << iteration.first << "," << std::get<0>(iteration.second) << "," << std::get<1>(iteration.second) << std::endl;
            }
        }
    }
    fout.close();

    // Write Duplicate File
    pname = output_pathname + ".duplicates.csv";
    BOOST_LOG_TRIVIAL(debug) << "Opening " << pname;
    fout.open(pname.c_str());
    fout << "SectorId,NumWaypoints,Iteration,NumberDuplicates" << std::endl;
    
    for( const auto& sec : m_duplicate_info )
    {
        for( const auto& wp : sec.second )
        {
            for( const auto& iteration : wp.second )
            {
                fout << sec.first << "," << wp.first << "," << iteration.first << "," << iteration.second << std::endl;
            }
        }
    }
    fout.close();
}
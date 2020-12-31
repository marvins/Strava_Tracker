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
/*          Constructor         */
/********************************/
Stats_Aggregator::Stats_Aggregator( const std::string& output_pathname )
  : m_output_pathname( output_pathname )
{
}

/********************************/
/*          Destructor          */
/********************************/
Stats_Aggregator::~Stats_Aggregator()
{
    for( const auto& subsystem : m_timing_info )
    {
        BOOST_LOG_TRIVIAL(info) << subsystem.second.To_String( "Subsystem: " + subsystem.first, "sec" );
    }

    Stop_Writer();
}

/****************************************/
/*          Start the writer            */
/****************************************/
void Stats_Aggregator::Start_Writer()
{
    // Write the initial iteration file
    std::ofstream fout;
    auto pname = m_output_pathname + ".iteration.csv";
    BOOST_LOG_TRIVIAL(debug) << "Opening " << pname;
    fout.open( pname.c_str() );
    fout << "SectorId,NumWaypoints,Iteration,BestFitness,IterationTimeSec" << std::endl;
    fout.close();

    // Write the initial duplicate file
    pname = m_output_pathname + ".duplicates.csv";
    BOOST_LOG_TRIVIAL(debug) << "Opening " << pname;
    fout.open( pname.c_str() );
    fout << "SectorId,NumWaypoints,Iteration,NumberDuplicates" << std::endl;
    fout.close();

    // Start the main thread
    m_okay_to_run = true;
    m_write_thread = std::thread( &Stats_Aggregator::Write_Stats_Info, this );
}

/************************************/
/*          Kill the Writer         */
/************************************/
void Stats_Aggregator::Stop_Writer()
{
    m_okay_to_run = false;
    if( m_write_thread.joinable() )
    {
        m_write_thread.join();
    }
}

/****************************************/
/*          Report Timing Info          */
/****************************************/
void Stats_Aggregator::Report_Timing( const std::string&  subsystem,
                                      double              elapsed_time )
{
    std::lock_guard<std::mutex> lck(m_timing_mtx);
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
    std::lock_guard<std::mutex> lck(m_iter_mtx);
    BOOST_LOG_TRIVIAL(debug) << "Logging Iteration Complete. Sector: " << sector_id
                             << ", Waypoints: " << num_waypoints << ", Iteration: " 
                             << iteration_number << ", Fitness: " << best_fitness 
                             << std::fixed << ", Time: " << iteration_time_ms;

    // Create new file entry [SectorId,NumWaypoints,Iteration,BestFitness,IterationTimeSec]
    std::stringstream sout;
    sout << sector_id << "," << num_waypoints << "," << iteration_number << "," << std::fixed << best_fitness << "," << iteration_time_ms;
    m_iteration_info.push_back(sout.str());
}

/************************************************/
/*          Report a Duplicate Entry            */
/************************************************/
void Stats_Aggregator::Report_Duplicate_Entry( const std::string& sector_id,
                                               size_t             num_waypoints,
                                               size_t             iteration_number,
                                               size_t             number_duplicates )
{
    std::lock_guard<std::mutex> lck(m_dup_mtx);

    // Create new file entry [SectorId,NumWaypoints,Iteration,NumberDuplicates]
    std::stringstream sout;
    sout << sector_id << "," << num_waypoints << "," << iteration_number << "," << number_duplicates;
    m_duplicate_info.push_back(sout.str());
}

/****************************************/
/*          Write Stats Info            */
/****************************************/
void Stats_Aggregator::Write_Stats_Info()
{
    while( m_okay_to_run )
    {
        BOOST_LOG_TRIVIAL(debug) << "Starting File Write Cycle";
        // Write Iteration Info
        {
            BOOST_LOG_TRIVIAL(debug) << "Checking Iteration Queue";

            // If there is content to write, open the file and write it
            std::lock_guard<std::mutex> lck(m_iter_mtx);
            if( !m_iteration_info.empty() )
            {
                // Re-Open the file
                std::ofstream fout;
                auto pname = m_output_pathname + ".iteration.csv";
                BOOST_LOG_TRIVIAL(debug) << "Opening " << pname;
                fout.open( pname.c_str(), std::ios_base::app );

                // Write the new content
                while( !m_iteration_info.empty() )
                {
                    fout << m_iteration_info.front() << std::endl;
                    m_iteration_info.pop_front();
                }

                // Clean up
                fout.close();
            }
        }

        // Write Duplicate Info
        {
            BOOST_LOG_TRIVIAL(debug) << "Checking Duplicate Queue";

            // If there is content to write, open the file and write it
            std::lock_guard<std::mutex> lck(m_dup_mtx);
            if( !m_duplicate_info.empty() )
            {
                // Re-Open the file
                std::ofstream fout;
                auto pname = m_output_pathname + ".duplicates.csv";
                BOOST_LOG_TRIVIAL(debug) << "Opening " << pname;
                fout.open( pname.c_str(), std::ios_base::app );

                // Write the new content
                while( !m_duplicate_info.empty() )
                {
                    fout << m_duplicate_info.front() << std::endl;
                    m_duplicate_info.pop_front();
                }

                // Clean up
                fout.close();
            }
        }

        // Sleep for a bit
        std::this_thread::sleep_for( std::chrono::seconds(5) );
    }
    BOOST_LOG_TRIVIAL(debug) << "Closing Stats Write Queue";
}
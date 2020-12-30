/**
 * @file    Genetic_Algorithm.hpp
 * @author  Marvin Smith
 * @date    12/19/2020
 */
#pragma once

// C++ Libraries
#include <algorithm>
#include <functional>

// Project Libraries
#include "GA_Config.hpp"
#include "Exit_Condition.hpp"
#include "Stats_Aggregator.hpp"
#include "Thread_Pool.hpp"
#include "WaypointList.hpp" // REMOVE ME!

// Boost Libraries
#include <boost/log/trivial.hpp>

template <typename Phenotype>
class Genetic_Algorithm
{
    public:
        
        /**
         * @brief Constructor
         * @param config Configuration of the GA
         * @param population Initial population sample
         */
        Genetic_Algorithm( GA_Config                                     config,
                           std::vector<Phenotype>                        population,
                           std::function<Phenotype(const Phenotype&, 
                                                   const Phenotype&)>    crossover_algorithm,
                           std::function<void(Phenotype&)>               mutation_algorithm,
                           std::function<void(Phenotype&)>               random_algorithm,
                           std::function<void(const Phenotype&, size_t)> write_worker,
                           Stats_Aggregator&                             stats_aggregator,
                           bool                                          append_stats )
          : m_config( config ),
            m_population(population),
            m_crossover_algorithm(crossover_algorithm),
            m_mutation_algorithm(mutation_algorithm),
            m_random_algorithm(random_algorithm),
            m_write_worker(write_worker),
            m_aggregator(stats_aggregator),
            m_append_stats(append_stats)
        {
        }

        /**
         * @brief Run the GA
         */
        std::vector<Phenotype> Run( const std::string&    sector_id,
                                    int                   max_iterations = 1000,
                                    Exit_Condition::ptr_t exit_condition = std::make_shared<Exit_Condition>(),
                                    void*                 context_info   = nullptr )
        {
            // Compute the key population subset sizes
            size_t selection_size    = m_config.selection_rate * m_population.size();
            size_t preservation_size = std::max( 1.0, m_config.preservation_rate * m_population.size());
            size_t mutation_size     = std::max( 0.0, m_config.mutation_rate * m_population.size());
            size_t expected_population_size = m_population.size();

            {
                // Capture a log of the configuration structure as this is really important
                std::stringstream sout;
                sout << "Population Size   : " << m_population.size() << std::endl;
                sout << "Preservation Size : " << preservation_size << ", Rate: " << m_config.preservation_rate << std::endl;
                sout << "Selection Size    : " << selection_size << ", Rate: " << m_config.selection_rate << std::endl;
                sout << "Mutation Size     : " << mutation_size << ", Rate: " << m_config.mutation_rate << std::endl;
                sout << "Crossover Size    : " << m_population.size() - (preservation_size + selection_size) << std::endl;
                BOOST_LOG_TRIVIAL(debug) << sout.str();
            }

            // CHeck Population Validity
            for( const auto& p : m_population )
            {
                assert( p.Get_DNA().size() == p.Get_DNA_Expected_Size( ) );
            }

            // Run the iterations
            for( int iteration = 0; iteration < max_iterations; iteration++ )
            {
                assert( expected_population_size == m_population.size() );
                BOOST_LOG_TRIVIAL(debug) << "Starting Iteration " << iteration << " of " << max_iterations;
                auto start_loop_time = std::chrono::steady_clock::now();

                // Define a subset for Selection
                auto selectionStartIdx = preservation_size;
                auto selectionStopIdx  = preservation_size + selection_size;

                // Run Crossover on random pairs
                for( int cidx = selectionStopIdx; cidx < m_population.size(); cidx++ )
                {
                    // Pick Crossover indices from the selection range
                    size_t idx1 = rand() % selectionStopIdx;
                    size_t idx2 = rand() % selectionStopIdx;
                    while( idx1 == idx2 )
                    {
                        idx2 = (rand() % selectionStopIdx - selectionStartIdx) + selectionStartIdx;
                    }
                    m_population[cidx] = m_crossover_algorithm( m_population[idx1],
                                                                m_population[idx2] );
                }

                // Run Mutation
                auto start_mutation = std::chrono::steady_clock::now();
                for( size_t midx = 0; midx < mutation_size; midx++ )
                {
                    // Do not run mutation on the preservation set!
                    size_t mutationIdx = rand() % (m_population.size() - selectionStartIdx) + selectionStartIdx;
                    m_mutation_algorithm( m_population[mutationIdx] ); 
                }
                auto mutation_time = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - start_mutation ).count()/1000.0;
                m_aggregator.Report_Timing( "Mutation", mutation_time );

                // Update Fitness Scores
                auto start_fitness = std::chrono::steady_clock::now();
                {
                    Thread_Pool pool( m_config.number_threads );
                    for( auto& member : m_population )
                    {
                        pool.enqueue_work([&]() {
                            member.Update_Fitness( context_info,
                                                   false,
                                                   m_aggregator );
                        });
                    }
                    auto fitness_time = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - start_fitness ).count()/1000.0;
                    m_aggregator.Report_Timing( "Initial Fitness Jobs", fitness_time );
                }
                auto fitness_time = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - start_fitness ).count()/1000.0;
                m_aggregator.Report_Timing( "Initial Fitness Full", fitness_time );

                //////////////////////////////////////////////////////
                //////////////////////////////////////////////////////
                // Randomize Unique Entries (No point in crossing-over yourself over and over)
                #if 1
                auto start_unique = std::chrono::steady_clock::now();
                std::sort( m_population.begin(), m_population.end() );
                auto end_of_unique_iter = std::unique( m_population.begin(), m_population.end() );

                // For the duplicates, create random entries
                for( ; end_of_unique_iter != m_population.end(); end_of_unique_iter++ )
                {
                    m_aggregator.Report_Duplicate_Entry( sector_id,
                                                         m_population.front().Get_Number_Waypoint(), 
                                                         iteration );
                    if( rand()%3 == 0 )
                    {
                        m_random_algorithm( *end_of_unique_iter );
                    }
                    else
                    {
                        size_t rvidx = rand() % selectionStopIdx;
                        end_of_unique_iter->Randomize_Vertices( m_population[rvidx] );
                    }
                }

                
                auto unique_time = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - start_unique ).count()/1000.0;
                m_aggregator.Report_Timing( "Unique Full", unique_time );

                // Update Fitness Scores
                start_fitness = std::chrono::steady_clock::now();
                {
                    Thread_Pool pool;
                    for( auto& member : m_population )
                    {
                        pool.enqueue_work([&]() {
                           member.Update_Fitness( context_info,
                                                  true,
                                                  m_aggregator );
                        });
                    }
                    fitness_time = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - start_fitness ).count()/1000.0;
                    m_aggregator.Report_Timing( "Second Fitness Jobs", fitness_time );
                }
                m_aggregator.Report_Timing( "Second Fitness Full", fitness_time );
                #endif
                //////////////////////////////////////////////////////
                //////////////////////////////////////////////////////

                // Sort the population one last time
                std::sort( m_population.begin(), m_population.end() );

                BOOST_LOG_TRIVIAL(debug) << "Iteration: " << iteration << ", Current Best Matches: " << Print_Population_List( m_population, 10 );

                auto iter_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - start_loop_time );
                m_aggregator.Report_Iteration_Complete( sector_id,
                                                        m_population.front().Get_Number_Waypoint(), 
                                                        iteration,
                                                        m_population.front().Get_Fitness(),
                                                        iter_time_ms.count()/1000.0 );

                // Write Stats Data
                m_aggregator.Write_Stats_Info( m_config.stats_output_pathname,
                                               m_append_stats );

                // Check Exit Condition
                if( exit_condition->Check_Exit( m_population.front().Get_Fitness() ) )
                {
                    break;
                }

                // Write Latest Results
                m_write_worker( m_population.front(), iteration );
            }
            return m_population;
        }

    private:

        // Configuration
        GA_Config m_config;

        // Active Population
        std::vector<Phenotype> m_population;

        // Crossover Algorithm
        std::function<Phenotype(const Phenotype&, const Phenotype&)> m_crossover_algorithm;

        // Mutation Algorithm
        std::function<void(Phenotype&)> m_mutation_algorithm;

        // Random Algorithm
        std::function<void(Phenotype&)> m_random_algorithm;

        // Intermediate Write Worker
        std::function<void(const Phenotype&, size_t)> m_write_worker;

        // Stats Aggregation Class
        Stats_Aggregator& m_aggregator;

        // Append the Stats File 
        bool m_append_stats { false };
        
}; // End of Genetic_Algorithm Class
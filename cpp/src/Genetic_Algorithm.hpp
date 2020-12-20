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
        Genetic_Algorithm( GA_Config                                  config,
                           std::vector<Phenotype>                     population,
                           std::function<Phenotype(const Phenotype&, 
                                                   const Phenotype&)> crossover_algorithm,
                           std::function<void(Phenotype&)>            mutation_algorithm )
          : m_config( config ),
            m_population(population),
            m_crossover_algorithm(crossover_algorithm),
            m_mutation_algorithm(mutation_algorithm)
        {
        }

        /**
         * @brief Run the GA
         */
        std::vector<Phenotype> Run( int                   max_iterations = 1000,
                                    Exit_Condition::ptr_t exit_condition = std::make_shared<Exit_Condition>(),
                                    void*                 context_info   = nullptr )
        {
            // Compute the key population subset sizes
            size_t selection_size    = m_config.selection_rate * m_population.size();
            size_t preservation_size = m_config.preservation_rate * m_population.size();
            size_t mutation_size     = m_config.mutation_rate * m_population.size();
            BOOST_LOG_TRIVIAL(debug) << "SelectionSize: " << selection_size << ", PreservationSize: " 
                                     << preservation_size << ", MutationSize: " << mutation_size;

            // Run the iterations
            for( int iteration = 0; iteration < max_iterations; iteration++ )
            {
                BOOST_LOG_TRIVIAL(debug) << "Starting Iteration " << iteration << " of " << max_iterations;

                // Define a subset for Selection
                auto selectionStartIdx = preservation_size;
                auto selectionStopIdx  = preservation_size + selection_size;

                // Run Crossover on random pairs
                for( int cidx = selectionStopIdx; cidx < m_population.size(); cidx++ )
                {
                    // Pick Crossover indices from the selection range
                    size_t idx1 = (rand() % (selectionStopIdx - selectionStartIdx)) + selectionStartIdx;
                    size_t idx2 = (rand() % (selectionStopIdx - selectionStartIdx)) + selectionStartIdx;
                    while( idx1 == idx2 )
                    {
                        idx2 = (rand() % selectionStopIdx - selectionStartIdx) + selectionStartIdx;
                    }
                    m_population[cidx] = m_crossover_algorithm( m_population[idx1],
                                                                m_population[idx2] );
                }

                // Run Mutation
                for( size_t midx = 0; midx < mutation_size; midx++ )
                {
                    // Do not run mutation on the preservation set!
                    size_t mutationIdx = rand() % (m_population.size() - preservation_size) + preservation_size;
                    m_mutation_algorithm( m_population[mutationIdx] ); 
                }

                // Update Fitness Scores
                for( auto& member : m_population )
                {
                    member.Update_Fitness( context_info );
                }

                // Sort the population
                std::sort( m_population.begin(),
                           m_population.end() );
                
                BOOST_LOG_TRIVIAL(debug) << "Iteration: " << iteration << ", Current Best Matches: " << Print_Population_List( m_population, 10 );

                // Check Exit Condition
                if( exit_condition->Check_Exit( m_population.front().Get_Fitness() ) )
                {
                    break;
                }
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
        
}; // End of Genetic_Algorithm Class
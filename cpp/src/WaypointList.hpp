/**
 * @file    WaypointList.hpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#pragma once

// C++ Libraries
#include <functional>
#include <string>
#include <vector>

// Project Libraries
#include "Geometry.hpp"

/**
 * @class WaypointList
 * Phenotype for the Genetic Algorithm
 */
class WaypointList
{
    public:

        /// Crossover Function Type
        typedef std::function<WaypointList(const WaypointList&,const WaypointList&)> crossover_func_tp;
        
        /// Mutation Function Type
        typedef std::function<void(WaypointList&)> mutation_func_tp;

        /// Randomization Function Type
        typedef std::function<void(WaypointList&)> random_func_tp;

        /**
         * @brief Build a new Phenotype from the string
         */
        WaypointList( std::string dna,
                      size_t      number_points,
                      size_t      x_digits,
                      size_t      y_digits );

        /**
         * @brief Get the Number of Waypoints
         */
        size_t Get_Number_Waypoint() const
        {
            return m_number_points;
        }

        /**
         * @brief Get the Fitness Score
         */
        double Get_Fitness() const;

        /**
         * @brief Set the Fitness
         * @note Only use for testing.
         */
        void Set_Fitness( double fitness );

        /**
         * @brief Compute a new fitness score
         */
        void Update_Fitness( void* context_info,
                             bool  check_fitness );

        /**
         * @brief Get the vertices from the dna strand
         */
        std::vector<Point<double>> Get_Vertices() const;

        /**
         * @brief Randomize the Vertices
        */
        void Randomize_Vertices( const WaypointList& wp );

        /**
         * @brief Convert to loggable string
         */
        std::string To_String( bool show_vertices = false ) const;

        /**
         * @brief Less-Than Operator
         */
        bool operator < ( const WaypointList& rhs ) const;

        /**
         * @brief Equivalent Operator
         */
        bool operator == ( const WaypointList& rhs ) const;

        /**
         * @brief Create a random waypoint list
         */
        static WaypointList Create_Random( size_t number_points,
                                           size_t max_x,
                                           size_t max_y );

        /**
         * @brief Perform Crossover on Waypoint List
         * @param wp1 First waypoint list
         * @param wp2 Second waypoint list
         * @return New waypoint list.
         */
        static WaypointList Crossover( const WaypointList& wp1, 
                                       const WaypointList& wp2 );

        /**
         * @brief Perform Mutation on a Waypoint
         */
        static void Mutation( WaypointList& wp );

        /**
         * @brief Perform Randomization on a Waypoint
         */
        static void Randomize( WaypointList& wp );

    private:

        // The actual phenotype the GA will use
        std::string m_dna;

        // The Fitness Score (Lower is better in this GA)
        double m_fitness;
        double m_point_score { 0 };
        double m_length_score { 0 };
        double m_density_score { 0 };

        // Number Points
        size_t m_number_points;

        /// Precision
        size_t m_max_x;
        size_t m_max_y;
        size_t m_x_digits;
        size_t m_y_digits;

}; // End of the WaypointList Class

/**
 * @brief Build a random population
 * @param population_size 
 * @param number_points Number of points in the waypoint list
 * @param max_x Max distance needed for easting
 * @param max_y Max distance needed for northing
 */
std::vector<WaypointList> Build_Random_Waypoints( size_t population_size,
                                                  size_t number_points,
                                                  size_t max_x,
                                                  size_t max_y );

/**
 * @brief Logging-friendly way to print the population list
 * @param max_values If zero, will print entire list.  If > pop size, then will print entire list.
 */
std::string Print_Population_List( const std::vector<WaypointList>& population,
                                   size_t                           max_values = 0 );
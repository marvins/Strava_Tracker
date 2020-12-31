/**
 * @file    WaypointList.hpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#pragma once

// C++ Libraries
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

// Project Libraries
#include "Geometry.hpp"
#include "Stats_Aggregator.hpp"

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
         * @param dna Text to use as dna strand
         * @param number_points Number of points to solve for
         * @param max_x Max X Value for range computation.
         * @param max_y Max Y Value for range computation
         * @param start_point Normalized starting coordinate
         * @param end_point Normalized ending coordinate.
         */
        WaypointList( std::string  dna,
                      size_t       number_points,
                      size_t       max_x,
                      size_t       max_y,
                      const Point& start_point,
                      const Point& end_point );

        /**
         * @brief Create a WaypointList from normalized vertices.
         */
        WaypointList( const std::vector<Point>& waypoints,
                      size_t                    max_x,
                      size_t                    max_y,
                      const Point&              start_point,
                      const Point&              end_point );

        /**
         * @brief Get the Number of Waypoints
         */
        size_t Get_Number_Waypoint() const
        {
            return m_number_points;
        }

        /**
         * @brief Get the DNA String
         */
        std::string Get_DNA() const;

        /**
         * @brief Get the expected DNA Size
         */
        size_t Get_DNA_Expected_Size() const;

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
        void Update_Fitness( void*             context_info,
                             bool              check_fitness,
                             Stats_Aggregator& aggregator );

        /**
         * @brief Get the Max X Value
         */
        size_t Get_Max_X() const;

        /**
         * @brief Get the Max Y Value
         */
        size_t Get_Max_Y() const;

        /**
         * @brief Get the Starting Point
         */
        Point Get_Start_Point() const;

        /**
         * @brief Get the Ending Point
         */
        Point Get_End_Point() const;
        
        /**
         * @brief Get the vertices from the dna strand
         */
        std::vector<Point> Get_Vertices( bool skip_ends = false) const;

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
        static WaypointList Create_Random( size_t       number_points,
                                           size_t       max_x,
                                           size_t       max_y,
                                           const Point& start_point,
                                           const Point& end_point );

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

        // Number Points
        size_t m_number_points;

        /// Precision
        size_t m_max_x;
        size_t m_max_y;
        size_t m_x_digits;
        size_t m_y_digits;

        Point m_start_point;
        Point m_end_point;

}; // End of the WaypointList Class

/**
 * @brief Build a random population
 * @param population_size 
 * @param number_points Number of points in the waypoint list
 * @param max_x Max distance needed for easting
 * @param max_y Max distance needed for northing
 * @param start_point Starting point in normalized coordinates.
 * @param end_point Ending point in normalized coordinates.
 */
std::vector<WaypointList> Build_Random_Waypoints( size_t       population_size,
                                                  size_t       number_points,
                                                  size_t       max_x,
                                                  size_t       max_y,
                                                  const Point& start_point,
                                                  const Point& end_point );

/**
 * @brief Logging-friendly way to print the population list
 * @param max_values If zero, will print entire list.  If > pop size, then will print entire list.
 */
std::string Print_Population_List( const std::vector<WaypointList>& population,
                                   size_t                           max_values = 0 );

/**
 * @brief Write the population data to disk
 * 
 * @param population
 * @param sector_id
 * @param output_path
 * @param append_to_file
 */
void Write_Population( const std::vector<WaypointList>& population,
                       const std::string&               sector_id,
                       const std::filesystem::path&     output_path,
                       bool                             append_to_file );

/**
 * @brief Load the population file.
 */
std::map<int,std::vector<WaypointList>> Load_Population( const std::filesystem::path& input_pathname,
                                                         size_t                       min_waypoints,
                                                         size_t                       max_waypoints,
                                                         size_t                       population_size );
/**
 * @brief Seed the population file.
 */
std::map<int,std::vector<WaypointList>> Seed_Population( const std::vector<Point>& dataset_points,
                                                         size_t                    min_waypoints,
                                                         size_t                    max_waypoints,
                                                         size_t                    population_size,
                                                         size_t                    max_x,
                                                         size_t                    max_y,
                                                         const Point&              start_point,
                                                         const Point&              end_point );

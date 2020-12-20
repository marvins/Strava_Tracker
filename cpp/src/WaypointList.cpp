/**
 * @file    WaypointList.cpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include "WaypointList.hpp"

// Project Libraries
#include "DB_Point.hpp"
#include "Geometry.hpp"

// C++ Libraries
#include <sstream>

/********************************/
/*          Constructor         */
/********************************/
WaypointList::WaypointList( std::string dna,
                            size_t      number_points,
                            size_t      x_digits,
                            size_t      y_digits )
 : m_dna(dna),
   m_fitness( -1 ),
   m_number_points(number_points),
   m_x_digits(x_digits),
   m_y_digits(y_digits)
{
}

/********************************************/
/*          Get the Fitness Value           */
/********************************************/
double WaypointList::Get_Fitness() const
{
    return m_fitness;
}

/************************************************************************/
/*           Update the Fitness Score Against the Point List            */
/************************************************************************/
void WaypointList::Update_Fitness( void* context_info )
{
    // Skip everything if the fitness is still valid
    if( m_fitness >= 0 )
    {
        return;
    }

    // Cast to the point list
    auto point_list = *reinterpret_cast<std::vector<DB_Point>*>( context_info );

    // Get the vertex list
    auto vertices = Get_Vertices();

    // Map each reference point against it's "best-fit" line-segment
    m_fitness = 0;
    for( const auto& point : point_list )
    {
        m_fitness += Find_Best_Segment_Error( Point( point.x_norm, point.y_norm ), vertices );
    }
}

/************************************************/
/*      Get the vertex list from the dna        */
/************************************************/
std::vector<Point<double>> WaypointList::Get_Vertices() const
{
    std::vector<Point<double>> waypoints( m_number_points );

    for( size_t i=0; i<m_number_points; i++ )
    {
        waypoints[i].x() = std::stod(m_dna.substr( i * (m_x_digits + m_y_digits), m_x_digits ));
        waypoints[i].y() = std::stod(m_dna.substr( i * (m_x_digits + m_y_digits) + m_x_digits, m_y_digits ));
    }

    return waypoints;
}

/****************************************/
/*          Less-Than Operator          */
/****************************************/
bool WaypointList::operator< ( const WaypointList& rhs ) const
{
    return ( m_fitness < rhs.m_fitness );
}

/****************************************************/
/*          Create a Random Waypoint List           */
/****************************************************/
WaypointList WaypointList::Create_Random( size_t number_points,
                                          size_t x_digits,
                                          size_t y_digits )
{
    size_t str_size = number_points * (x_digits + y_digits);
    std::string dna;
    for( size_t i=0; i<str_size; i++ )
    {
        dna.push_back( rand() % 10 + '0' );
    }

    return WaypointList( dna, number_points, x_digits, y_digits );
}

/********************************************************/
/*          Perform Crossover on WaypointLists          */
/********************************************************/
WaypointList WaypointList::Crossover( const WaypointList& wp1, 
                                      const WaypointList& wp2 )
{
    // Find anchor points  (First index cannot be on first position or last to allow second index to work)
    size_t idx1 = (rand() % (wp1.m_dna.size() - 3) + 1);

    // For now stick with single-point crossover
    auto output_dna = wp1.m_dna.substr(0, idx1) + wp2.m_dna.substr(idx1);
    return WaypointList( output_dna,
                         wp1.m_number_points,
                         wp1.m_x_digits,
                         wp1.m_y_digits );
}

/****************************************/
/*          Perform Mutation            */
/****************************************/
void WaypointList::Mutation( WaypointList& wp )
{
    // Pick a single entry to tweak
    wp.m_dna[rand() % wp.m_dna.size()] = rand() % 10 + '0';
    wp.m_fitness = -1;
}

/********************************************************/
/*          Create a Random Set of Waypoints            */
/********************************************************/
std::vector<WaypointList> Build_Random_Waypoints( size_t population_size,
                                                  size_t number_points,
                                                  size_t x_digits,
                                                  size_t y_digits )
{
    std::vector<WaypointList> output;
    for( size_t i=0; i<population_size; i++ )
    {
        output.push_back( WaypointList::Create_Random( number_points, 
                                                       x_digits, 
                                                       y_digits ) );
    }
    return output;
}

/************************************/
/*          Print to String         */
/************************************/
std::string WaypointList::To_String( bool show_vertices ) const
{
    std::stringstream sout;
    sout << "DNA: [" << m_dna << "], Fitness: " << std::fixed << m_fitness;
    if( show_vertices )
    {
        sout << std::endl;
        const auto& vertices = Get_Vertices();
        for( const auto& v : vertices )
        {
            sout << v.To_String() << std::endl;
        }
    }
    return sout.str();
}

/************************************************/
/*          Print the Population list           */
/************************************************/
std::string Print_Population_List( const std::vector<WaypointList>& population,
                                   size_t                           max_values )
{
    size_t print_size = population.size();
    if( max_values > 0 && max_values < population.size() )
    {
        print_size = max_values;
    }
    std::stringstream sout;
    sout << "Sample: " << max_values << " of " << population.size() << std::endl;
    for( size_t i=0; i<print_size; i++ )
    {
        sout << "  " << i << " -> " << population[i].To_String() << std::endl;
    }
    return sout.str(); 
}
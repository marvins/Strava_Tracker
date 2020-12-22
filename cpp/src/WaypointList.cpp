/**
 * @file    WaypointList.cpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include "WaypointList.hpp"

// Project Libraries
#include "Accumulator.hpp"
#include "DB_Point.hpp"
#include "Geometry.hpp"

// C++ Libraries
#include <algorithm>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>

static double global_min_segment_length = -1;

/********************************/
/*          Constructor         */
/********************************/
WaypointList::WaypointList( std::string dna,
                            size_t      number_points,
                            size_t      max_x,
                            size_t      max_y )
 : m_dna(dna),
   m_fitness( -1 ),
   m_number_points(number_points),
   m_max_x(max_x),
   m_max_y(max_y),
   m_x_digits(log10(max_x) + 1),
   m_y_digits(log10(max_y) + 1)
{
}

/********************************************/
/*          Get the Fitness Value           */
/********************************************/
double WaypointList::Get_Fitness() const
{
    return m_fitness;
}

/********************************************/
/*          Set the Fitness Value           */
/********************************************/
void WaypointList::Set_Fitness( double fitness )
{
    m_fitness = fitness;
}

/************************************************************************/
/*           Update the Fitness Score Against the Point List            */
/************************************************************************/
void WaypointList::Update_Fitness( void* context_info,
                                   bool  check_fitness )
{
    // Skip everything if the fitness is still valid
    if( check_fitness && m_fitness >= 0 )
    {
        return;
    }

    // Cast to the point list
    auto point_list = *reinterpret_cast<std::vector<DB_Point>*>( context_info );

    // Get the vertex list
    auto vertices = Get_Vertices();

    // Map each reference point against it's "best-fit" line-segment
    m_point_score = 0;
    std::map<int,int> segment_histogram;
    for( const auto& point : point_list )
    {
        m_point_score += Find_Best_Segment_Error( Point( point.x_norm, point.y_norm ), 
                                                  vertices,
                                                  segment_histogram );
    }
    // Normalize Fitness
    m_point_score /= point_list.size();

    m_segment_score = 0;
    for( size_t i=0; i<(vertices.size()-1); i++ )
    {
        m_segment_score += Point<double>::Distance_L2( vertices[i], vertices[i+1] );
    }
    if( global_min_segment_length < 0 )
    {
        global_min_segment_length = m_segment_score; 
    }
    m_segment_score = 100 * ( m_segment_score / global_min_segment_length );

    m_fitness = m_point_score + m_segment_score;
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
/*          Randomize Vertices          */
/****************************************/
void WaypointList::Randomize_Vertices( const WaypointList& wp )
{
    // Get the vertices
    auto verts = wp.Get_Vertices();

    std::shuffle( verts.begin(), verts.end(), std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()) );
    std::stringstream dna;
    for( size_t i=0; i<wp.m_number_points; i++ )
    {
        dna << std::setfill('0') << std::setw(m_x_digits) << verts[i].x();
        dna << std::setfill('0') << std::setw(m_y_digits) << verts[i].y();
    }
    m_dna = dna.str();
    assert( m_dna.size() == dna.str().size() );
    m_fitness = -1;
}

/****************************************/
/*          Less-Than Operator          */
/****************************************/
bool WaypointList::operator< ( const WaypointList& rhs ) const
{
    return ( m_fitness < rhs.m_fitness );
}

/*****************************************/
/*          Equivalent Operator          */
/*****************************************/
bool WaypointList::operator == ( const WaypointList& rhs ) const
{
    return ( m_dna == rhs.m_dna );
}

/****************************************************/
/*          Create a Random Waypoint List           */
/****************************************************/
WaypointList WaypointList::Create_Random( size_t number_points,
                                          size_t max_x,
                                          size_t max_y )
{
    size_t x_digits = log10(max_x) + 1;
    size_t y_digits = log10(max_y) + 1;

    std::stringstream dna;
    for( size_t i=0; i<number_points; i++ )
    {
        dna << std::setfill('0') << std::setw(x_digits) << rand() % max_x;
        dna << std::setfill('0') << std::setw(y_digits) << rand() % max_y;
    }

    return WaypointList( dna.str(), number_points, max_x, max_y );
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
                         wp1.m_max_x,
                         wp1.m_max_y );
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

/*********************************************/
/*          Perform Randomization            */
/*********************************************/
void WaypointList::Randomize( WaypointList& wp )
{
    std::stringstream dna;
    for( size_t i=0; i<wp.m_number_points; i++ )
    {
        dna << std::setfill('0') << std::setw(wp.m_x_digits) << rand() % wp.m_max_x;
        dna << std::setfill('0') << std::setw(wp.m_y_digits) << rand() % wp.m_max_y;
    }
    wp.m_dna = dna.str();
    wp.m_fitness = -1;
}

/********************************************************/
/*          Create a Random Set of Waypoints            */
/********************************************************/
std::vector<WaypointList> Build_Random_Waypoints( size_t population_size,
                                                  size_t number_points,
                                                  size_t max_x,
                                                  size_t max_y )
{
    std::vector<WaypointList> output;
    for( size_t i=0; i<population_size; i++ )
    {
        output.push_back( WaypointList::Create_Random( number_points, 
                                                       max_x, 
                                                       max_y ) );
    }
    return output;
}

/************************************/
/*          Print to String         */
/************************************/
std::string WaypointList::To_String( bool show_vertices ) const
{
    std::stringstream sout;
    sout << "DNA: [" << m_dna << "], Points: [" << m_number_points << "] Fitness: [" << std::fixed << m_fitness << "], Point Score: [" << m_point_score << "], Seg Score: [" << m_segment_score << "]";
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
/**
 * @file    WaypointList.cpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include "WaypointList.hpp"

// Project Libraries
#include "Accumulator.hpp"
#include "Context.hpp"
#include "Geometry.hpp"

// C++ Libraries
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <set>
#include <sstream>

// Boost Libraries
#include <boost/log/trivial.hpp>

static double global_min_segment_length = -1;

#define USE_POINT_DENSITY 0
#define USE_SEGMENT_DENSITY 1

/********************************/
/*          Constructor         */
/********************************/
WaypointList::WaypointList( std::string  dna,
                            size_t       number_points,
                            size_t       max_x,
                            size_t       max_y,
                            const Point& start_point,
                            const Point& end_point )
 : m_dna(dna),
   m_fitness( -1 ),
   m_number_points(number_points),
   m_max_x(max_x),
   m_max_y(max_y),
   m_x_digits(log10(max_x) + 1),
   m_y_digits(log10(max_y) + 1),
   m_start_point(start_point),
   m_end_point(end_point)
{
}

/********************************/
/*          Constructor         */
/********************************/
WaypointList::WaypointList( const std::vector<Point>& waypoints,
                            size_t                    max_x,
                            size_t                    max_y,
                            const Point&              start_point,
                            const Point&              end_point )
 : m_fitness( -1 ),
   m_number_points(waypoints.size()),
   m_max_x(max_x),
   m_max_y(max_y),
   m_x_digits(log10(max_x) + 1),
   m_y_digits(log10(max_y) + 1),
   m_start_point(start_point),
   m_end_point(end_point)
{
    std::stringstream dna;
    for( size_t i=0; i<m_number_points; i++ )
    {
        dna << std::setfill('0') << std::setw(m_x_digits) << (int)waypoints[i].x();
        dna << std::setfill('0') << std::setw(m_y_digits) << (int)waypoints[i].y();
    }
    m_dna = dna.str();
}

/****************************************/
/*          Get the DNA String          */
/****************************************/
std::string WaypointList::Get_DNA() const
{
    return m_dna;
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
void WaypointList::Update_Fitness( void*             context_info,
                                   bool              check_fitness,
                                   Stats_Aggregator& aggregator,
                                   double            new_min_seg_length )
{
    auto start_method = std::chrono::steady_clock::now();

    // Skip everything if the fitness is still valid
    if( check_fitness && m_fitness >= 0 )
    {
        return;
    }

    // Cast to the context
    auto context = *reinterpret_cast<Context*>( context_info );

    // Get the vertex list
    auto vertices = Get_Vertices();

    // Compute Point-Score:  Map each reference point against it's "best-fit" line-segment
    auto start_point = std::chrono::steady_clock::now();
    m_point_score = 0;
#if USE_POINT_DENSITY == 1
    std::map<int,int> segment_histogram;
    for( const auto& point : context.geo_point_list )
    {
        m_point_score += Find_Best_Segment_Error( point, 
                                                  vertices,
                                                  segment_histogram );
    }
    m_point_score /= context.point_list.size();
    auto point_timing = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::steady_clock::now() - start_point ).count() / 1000000.0;
    aggregator.Report_Timing( "Point Density Timing", point_timing );
#endif 

    // Compute Length Score
    m_length_score = 0;
    for( size_t i=0; i<(vertices.size()-1); i++ )
    {
        m_length_score += Point::Distance_L2( vertices[i], vertices[i+1] );
    }
    if( new_min_seg_length > 0 )
    {
        global_min_segment_length = new_min_seg_length;
    }
    else if( global_min_segment_length < 0 )
    {
        global_min_segment_length = m_length_score; 
        BOOST_LOG_TRIVIAL(debug) << "Global-Min-Segment-Length: " << global_min_segment_length << " meters";
    }
    m_length_score = 100 * ( m_length_score / global_min_segment_length );

    // Compute Density Score
    m_density_score = 0;
#if USE_SEGMENT_DENSITY == 1
    auto start_density = std::chrono::steady_clock::now();
    m_density_score = 100 * Get_Segment_Density( vertices,
                                                 context.point_quad_tree,
                                                 context.density_step_distance );
    auto density_timing = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::steady_clock::now() - start_density ).count() / 1000000.0;
    aggregator.Report_Timing( "Segment Density Timing", density_timing ); 
#endif                                                
    
    m_fitness = m_point_score + m_length_score + m_density_score;
    auto method_timing = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::steady_clock::now() - start_method ).count() / 1000000.0;
    aggregator.Report_Timing( "Fitness Method Timing", method_timing );
}

/****************************************/
/*          Get the Max X Value         */
/****************************************/
size_t WaypointList::Get_Max_X() const
{
    return m_max_x;
}

/****************************************/
/*          Get the Max Y Value         */
/****************************************/
size_t WaypointList::Get_Max_Y() const
{
    return m_max_y;
}

/****************************************/
/*          Get the Start Point         */
/****************************************/
Point WaypointList::Get_Start_Point() const
{
    return m_start_point;
}

/**************************************/
/*          Get the End Point         */
/**************************************/
Point WaypointList::Get_End_Point() const
{
    return m_end_point;
}

/************************************************/
/*      Get the vertex list from the dna        */
/************************************************/
std::vector<Point> WaypointList::Get_Vertices( bool skip_ends ) const
{
    std::vector<Point> waypoints( m_number_points +1 );
    
    if( !skip_ends )
    {
        waypoints[0] = m_start_point;
    }
    for( size_t i=0; i<m_number_points; i++ )
    {
        waypoints[i+1].x() = std::stod(m_dna.substr( i * (m_x_digits + m_y_digits), m_x_digits ));
        waypoints[i+1].y() = std::stod(m_dna.substr( i * (m_x_digits + m_y_digits) + m_x_digits, m_y_digits ));
    }
    if( !skip_ends )
    {
        waypoints.push_back( m_end_point );
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
WaypointList WaypointList::Create_Random( size_t       number_points,
                                          size_t       max_x,
                                          size_t       max_y,
                                          const Point& start_point,
                                          const Point& end_point )
{
    size_t x_digits = log10(max_x) + 1;
    size_t y_digits = log10(max_y) + 1;

    std::stringstream dna;
    for( size_t i=0; i<number_points; i++ )
    {
        dna << std::setfill('0') << std::setw(x_digits) << (rand() % max_x);
        dna << std::setfill('0') << std::setw(y_digits) << (rand() % max_y);
    }

    return WaypointList( dna.str(), 
                         number_points,
                         max_x,
                         max_y, 
                         start_point, 
                         end_point );
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
                         wp1.m_max_y,
                         wp1.m_start_point,
                         wp1.m_end_point );
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
std::vector<WaypointList> Build_Random_Waypoints( size_t       population_size,
                                                  size_t       number_points,
                                                  size_t       max_x,
                                                  size_t       max_y,
                                                  const Point& start_point,
                                                  const Point& end_point )
{
    std::vector<WaypointList> output;
    for( size_t i=0; i<population_size; i++ )
    {
        output.push_back( WaypointList::Create_Random( number_points, 
                                                       max_x, 
                                                       max_y,
                                                       start_point,
                                                       end_point ) );
    }
    return output;
}

/************************************/
/*          Print to String         */
/************************************/
std::string WaypointList::To_String( bool show_vertices ) const
{
    std::stringstream sout;
    sout << "DNA: [" << m_dna << "], Points: [" << m_number_points << "] Fitness: [" 
         << std::fixed << m_fitness << "], Point Score: [" << m_point_score 
         << "], Len Score: [" << m_length_score << "], Density Score: [" 
         << m_density_score << "]";
    if( show_vertices )
    {
        sout << std::endl;
        const auto& vertices = Get_Vertices();
        for( const auto& v : vertices )
        {
            std::string extra;
            if( v.x() > m_max_x || v.y() > m_max_y )
            {
                extra = ", OVER_LIMITS: ";
                if( v.x() > m_max_x )
                {
                    extra += "X by " + std::to_string(v.x() - m_max_x );
                }
                if( v.y() > m_max_y )
                {
                    extra += "Y by " + std::to_string(v.y() - m_max_y );
                }
            }
            sout << v.To_String() << extra << std::endl;
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

/************************************************/
/*          Write the Population Data           */
/************************************************/
void Write_Population( const std::vector<WaypointList>& population,
                       const std::filesystem::path&     output_path,
                       bool                             append_to_file )
{
    // Open the file
    std::ofstream fout;
    if( append_to_file )
    {
        fout.open( output_path, std::ios_base::app );
    }
    else
    {
        fout.open( output_path );
    }
    
    fout << "num_waypoints,population,dna,max_x,max_y,start_point_lat,start_point_lon,end_point_lat,end_point_lon,fitness" << std::endl;
    for( size_t i=0; i<population.size(); i++ )
    {
        fout << population[i].Get_Number_Waypoint() << "," << i << "," << population[i].Get_DNA() << ",";
        fout << population[i].Get_Max_X() << "," << population[i].Get_Max_Y() << ",";
        fout << population[i].Get_Start_Point().y() << "," << population[i].Get_Start_Point().x();
        fout << "," << population[i].Get_End_Point().x() << "," << population[i].Get_End_Point().y() << std::endl;
    }    

    // Close the file
    fout.close();
}

/************************************************/
/*          Load the Population Data            */
/************************************************/
std::map<int,std::vector<WaypointList>> Load_Population( const std::filesystem::path& input_pathname,
                                                         size_t                       min_waypoints,
                                                         size_t                       max_waypoints,
                                                         size_t                       population_size )
{
    throw std::runtime_error("Not implemented yet.");
}

/***********************************************/
/*          Seed the population file.          */
/***********************************************/
std::map<int,std::vector<WaypointList>> Seed_Population( const std::vector<Point>& dataset_points,
                                                         size_t                    min_waypoints,
                                                         size_t                    max_waypoints,
                                                         size_t                    population_size,
                                                         size_t                    max_x,
                                                         size_t                    max_y,
                                                         const Point&              start_point,
                                                         const Point&              end_point )
{
    std::map<int,std::vector<WaypointList>> output;

    double seed_ratio = 0.5;
    size_t seed_count = population_size * seed_ratio;

    // Iterate over the waypoint range
    for( size_t wp = min_waypoints; wp <= max_waypoints; wp++ )
    {
        output[wp] = std::vector<WaypointList>();
        for( size_t pop_id=0; pop_id < population_size; pop_id++ )
        {
            // Create a vertex list from a distribution of the points
            std::vector<Point> vertex_list;
            if( pop_id == 0 ) // Just to be safe, use an even distribution on the first entry
            {
                for( size_t x=0; x<wp; x++ )
                {
                    size_t tidx = std::min( dataset_points.size()-1, 
                                            std::max( (size_t)0, (size_t)(x * dataset_points.size() / wp )));
                    vertex_list.push_back( dataset_points[tidx] );
                }
            }
            else if( pop_id < seed_count )
            {
                // Create a random list of indeces and load into a set (sets filter duplicates and auto-sort)
                std::set<size_t> idx_list;
                while( idx_list.size() < wp )
                {
                    idx_list.insert( rand() % dataset_points.size() );
                }

                // Build the points
                for( const auto& tidx : idx_list )
                {
                    vertex_list.push_back( dataset_points[tidx] );
                }
            }
            else
            {
                auto temp_wp = WaypointList::Create_Random( wp, 
                                                            max_x,
                                                            max_y,
                                                            start_point,
                                                            end_point );
                vertex_list = temp_wp.Get_Vertices( true );
            }

            output[wp].emplace_back( vertex_list,
                                     max_x,
                                     max_y,
                                     start_point,
                                     end_point );
        }
    }

    return output;
}
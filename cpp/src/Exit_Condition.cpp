/**
 * @file    Exit_Condition.cpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include "Exit_Condition.hpp"

// Boost Libraries
#include <boost/log/trivial.hpp>

// C++ Libraries
#include <cmath>
#include <limits>

/******************************/
/*        Constructor         */
/******************************/
Exit_Condition::Exit_Condition( size_t max_matches,
                                double eps )
  : m_current_fitness(std::numeric_limits<double>::max()),
    m_max_matches( max_matches ),
    m_eps( eps )
{
}

/****************************************/
/*      Check the Exit Condition        */
/****************************************/
bool Exit_Condition::Check_Exit( double fitness )
{
    if( std::fabs(fitness - m_current_fitness) < m_eps )
    {
        m_counter++;
        BOOST_LOG_TRIVIAL(debug) << "No improvement in " << m_counter << " iterations";
        if( m_counter >= m_max_matches )
        {
            BOOST_LOG_TRIVIAL(info) << "Reached max fitness match count. Exiting.";
            return true;
        }
    }
    else
    {
        // Check for sanity purposes
        if( m_current_fitness < fitness )
        {
            BOOST_LOG_TRIVIAL(warning) << "You are attempting to update the fitness with a worse value. Current: " 
                                       << m_current_fitness << ", New: " << fitness;
        }
        m_current_fitness = fitness;
        m_counter = 0;   
    }
    return false;
}

/****************************************/
/*          Get the Max Matches         */
/****************************************/
size_t Exit_Condition::Get_Max_Matches() const
{
    return m_max_matches;
}

/********************************/
/*          Get the EPS         */
/********************************/
double Exit_Condition::Get_EPS() const
{
    return m_eps;
}
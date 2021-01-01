/**
 * @file    Exit_Condition.hpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#pragma once

// C++ Libraries
#include <memory>

class Exit_Condition
{
    public:

        typedef std::shared_ptr<Exit_Condition> ptr_t;

        /**
         * @brief Parameterized Constructor
         */
        Exit_Condition( size_t max_matches,
                        double eps );

        /**
         * @brief Check the Fitness Score if this warrants an early exit
         */
        virtual bool Check_Exit( double fitness );

        /**
         * @brief Get the Max Matches
         */
        size_t Get_Max_Matches() const;

        /**
         * @brief Get the EPS
         */
        double Get_EPS() const;

    private:

        /// Current Fitness Score
        double m_current_fitness { -1 };

        /// Number of times at the current fitness level
        size_t m_counter { 0 };

        /// Max Number of Matches
        size_t m_max_matches { 1000 };

        /// Epsilon for measuring a "match"
        double m_eps;

}; // End of Exit Condition

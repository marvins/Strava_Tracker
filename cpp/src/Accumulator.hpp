/**
 * @file    Accumulator.hpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#pragma once

// Boost Libraries
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/sum.hpp>
#include <boost/accumulators/statistics/variance.hpp>

// C++ Libraries
#include <sstream>

namespace ba = boost::accumulators;

/**
 * @brief Utility for aggregating stats from large datasets. 
 */
template <typename TP>
class Accumulator
{
    public:

        /**
         * @brief Add a sample to the accumulator
         */
        void Insert( const TP& new_sample )
        {
            m_acc( new_sample );
            m_count++;
        }

        /**
         * @brief Get the Max Value
         */
        TP Get_Max() const
        {
            return boost::accumulators::max( m_acc );
        }

        /**
         * @brief Get the Min Value
         */
        TP Get_Min() const
        {
            return boost::accumulators::min( m_acc );
        }

        /**
         * @brief Get the Mean Value
         */
        TP Get_Mean() const
        {
            return boost::accumulators::mean( m_acc );
        }

        /**
         * @brief Get the Count
         */
        size_t Get_Count() const
        {
            return m_count;
        }

        /**
         * @brief Get the sum of items. 
         */
        TP Get_Sum() const
        {
            return ba::sum( m_acc );
        }

        /**
         * @brief Get the Variance
         */
        TP Get_Variance() const
        {
            return ba::variance( m_acc );
        }

        std::string To_String( const std::string& message,
                               const std::string& units ) const
        {
            std::stringstream sout;
            sout << message << std::endl;
            sout << "  - Count : " << std::fixed << Get_Count() << std::endl;
            sout << "  - Min   : " << std::fixed << Get_Min() << " " << units << std::endl;
            sout << "  - Mean  : " << std::fixed << Get_Mean() << " " << units << std::endl;
            sout << "  - Max   : " << std::fixed << Get_Max() << " " << units << std::endl;
            sout << "  - StdDev: " << std::fixed << sqrt(Get_Variance()) << " " << units << std::endl;
            sout << "  - Var   : " << std::fixed << Get_Variance() << " " << units << std::endl;
            sout << "  - Sum   : " << std::fixed << Get_Sum() << " " << units << std::endl;
            return sout.str();
        }

    private:

        typedef ba::stats< ba::tag::mean, ba::tag::min, ba::tag::max, ba::tag::sum, ba::tag::variance > STATS_SET;

        /// Accumulator Set
        ba::accumulator_set<TP, STATS_SET > m_acc;
        size_t m_count { 0 };

}; // End of Accumulator Class
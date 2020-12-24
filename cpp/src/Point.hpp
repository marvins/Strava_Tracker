/**
 * @file    Point.hpp
 * @author  Marvin Smith
 * @date    12/23/2020
*/
#pragma once

// C++ Libraries
#include <array>
#include <cmath>
#include <sstream>
#include <string>

/**
 * @class Point
 * @brief Utility Class for Point Operations
*/
template <typename TP, size_t Dims>
class Point_
{
    public:

        // Datatype Enumeration
        typedef TP datatype;

        // Dimensions
        static constexpr size_t N = Dims;

        /**
         * @brief Function Operator
        */
        TP operator[]( size_t idx ) const
        {
            return m_data[idx];
        }

        /**
         * @brief Function Operator
        */
        TP& operator[]( size_t idx )
        {
            return m_data[idx];
        }

        /**
         * @brief Get X Value
         */
        TP x() const
        {
            return m_data[0];
        }
        TP& x() 
        {
            return m_data[0];
        }

        /**
         * @brief Get Y Value
         */
        TP y() const
        {
            return m_data[1];
        }
        TP& y() 
        {
            return m_data[1];
        }

        /**
         * @brief Size of the point
        */
        size_t Size() const
        {
            return N;
        }

        /**
         * @brief Add in-place operator
         */
        Point_<TP,Dims>& operator += ( const Point_<TP,Dims>& rhs )
        {
            for( size_t i=0; i<Size(); i++ )
            {
                m_data[i] += rhs.m_data[i];
            }
            return *this;
        }

        /**
         * @brief Subtract in-place operator
         */
        Point_<TP,Dims>& operator -= ( const Point_<TP,Dims>& rhs )
        {
            for( size_t i=0; i<Size(); i++ )
            {
                m_data[i] -= rhs.m_data[i];
            }
            return *this;
        }

        /**
         * @file Division in-place operator
        */
        template <typename ScalarType>
        Point_<TP,Dims>& operator *= ( ScalarType s )
        {
            for( auto& i : m_data )
            {
                i *= s;
            }
            return (*this);
        }

        /**
         * @file Division in-place operator
        */
        template <typename ScalarType>
        Point_<TP,Dims>& operator /= ( ScalarType s )
        {
            for( auto& i : m_data )
            {
                i /= s;
            }
            return (*this);
        }

        /**
         * @brief Distance (L2 Norm)
         * @param p1 First Point
         * @param p2 Second Point
         */
        static double Distance_L2( const Point_<TP,Dims>& p1,
                                   const Point_<TP,Dims>& p2 )
        {
            return (p1 - p2).Mag();
        }

        /**
         * @brief Compute the Max of the two points
         *
         * @param[in] point1 First point to evaluate.
         * @param[in] point2 Second point to evaluate.
         *
         * @return Max of the two points
         */
        static Point_<TP,Dims> Max( const Point_<TP,Dims>& point1,
                                    const Point_<TP,Dims>& point2 )
        {
            // Create output point
            Point_<TP,Dims> output;

            // Run std::max on each component
            for( size_t i=0; i<Dims; i++ )
            {
                output[i] = std::max( point1[i], point2[i] );
            }
            return output;
        }
        
        /**
         * @brief Compute the Min of the two points
         *
         * @param[in] point1 First point to evaluate.
         * @param[in] point2 Second point to evaluate.
         *
         * @return Min of the two points
         */
        static Point_<TP,Dims> Min( const Point_<TP,Dims>& point1,
                                    const Point_<TP,Dims>& point2 )
        {
            // Create output point
            Point_<TP,Dims> output;

            // Run std::min on each component
            for( size_t i=0; i<Dims; i++ )
            {
                output[i] = std::min( point1[i], point2[i] );
            }
            return output;
        }

        /**
         * @brief Perform Linear Interpolation
        */
        static Point_<TP,Dims> LERP( const Point_<TP,Dims>& pt1, 
                                     const Point_<TP,Dims>& pt2,
                                     double                 ratio )
        {
            return (((1-ratio) * pt1) + (ratio * pt2));
        }

        /**
         * @brief Get the Magnitude Squared.
         *
         * @return Sum of the elements squared.
         */
        double Mag2() const
        {
            double sum = 0;
            for( size_t i=0; i<Size(); i++ )
            {
                sum += (m_data[i]*m_data[i]);
            }
            return sum;
        }

        /**
         * @brief Get the Magnitude
         *
         * @return Square root of Mag2.
         */
        double Mag() const
        {
            if( std::fabs(Mag2()) < 0.000000001 ){
                return 0;
            }
            return std::sqrt(Mag2());
        }

        /**
         * @brief Normalize the vector.
         *
         * @return Vector with a length of 1.
         */
        Point_<TP,Dims> Norm() const
        {
            // get the magnitude
            double mag = Mag();

            // copy the data
            Point_<TP,Dims> output = (*this);
            for( size_t i=0; i<Size(); i++ )
            {
                output[i] /= mag;
            }

            return output;
        }

        /**
         * @brief Dot Product.
         *
         * @param[in] p1 First point.
         * @param[in] p2 Second point.
         *
         * @return \f $\hat{p1} \cdot $\hat{p2}$
         */
        static double Dot( const Point_<TP,Dims>& p1, 
                           const Point_<TP,Dims>& p2 )
        {
            double sum = 0;
            for( size_t i=0; i<Dims; i++ )
            {
                sum += (p1[i] * p2[i]);
            }
            return sum;
        }

        /**
         * @brief Print to Log-Friendly String
         */
        std::string To_String() const
        {
            std::stringstream sout;
            sout << "Point: " << std::fixed << x() << ", " << y();
            return sout.str();
        }

        // Underlying Datatype
        std::array<double,Dims> m_data;

}; // End of Point Class

/// 2d Point Template Alias
using Point = Point_<double,2>;

/**
 * @brief Create a 2D Point
 * @param x X-value 
 * @param y Y-value
 * @return New point created
*/
Point ToPoint2D( double x, double y );

/**
 * @brief Create a 2D Point
*/
template <typename TP>
Point_<TP,2> ToPoint2( TP x, TP y )
{
    auto p = Point_<TP,2>();
    p.m_data[0] = x;
    p.m_data[1] = y;
    return p;
}

/**
 * @brief Add 2 points
*/
template <typename TP, size_t Dims>
Point_<TP,Dims> operator + ( const Point_<TP,Dims>& p1, 
                             const Point_<TP,Dims>& p2 )
{
    Point_<TP,Dims> output;
    for( size_t i=0; i<p1.Size(); i++ )
    {
        output[i] = p1[i] + p2[i];
    }
    return output;
}

/**
 * @brief Subtract 2 points
*/
template <typename TP, size_t Dims>
Point_<TP,Dims> operator - ( const Point_<TP,Dims>& p1, 
                             const Point_<TP,Dims>& p2 )
{
    Point_<TP,Dims> output;
    for( size_t i=0; i<p1.Size(); i++ )
    {
        output[i] = p1[i] - p2[i];
    }
    return output;
}

/**
 * @brief Perform Multiplication with a Scalar
*/
template <typename TP, size_t Dims, typename S>
Point_<TP,Dims> operator * ( const Point_<TP,Dims>& pt, S scalar )
{
    Point_<TP,Dims> output;
    for( size_t i=0; i<pt.Size(); i++ )
    {
        output[i] = pt[i] * scalar;
    }
    return output;
}

/**
 * @brief Perform Multiplication with a Scalar
*/
template <typename TP, size_t Dims, typename S>
Point_<TP,Dims> operator * ( S scalar, const Point_<TP,Dims>& pt )
{
    Point_<TP,Dims> output;
    for( size_t i=0; i<pt.Size(); i++ )
    {
        output[i] = scalar * pt[i];
    }
    return output;
}
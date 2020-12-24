/**
 * @file    Rect.hpp
 * @author  Marvin Smith
 * @date    12/23/2020
*/
#pragma once

// Project Libraries
#include "Point.hpp"

/**
 * @class Rect
*/
template <typename TP>
class Rect_
{
    public:

        /**
         * @brief Default Constructor
        */
        Rect_()
         : m_min_corner( ToPoint2<TP>(0,0) )
        {
        }

        /**
         * @brief Parameterized Constructor
        */
        Rect_( const Point_<TP,2>& min_corner,
               TP                  width,
               TP                  height )
          : m_min_corner( min_corner ),
            m_width( width ),
            m_height( height )
        {
        }

        /**
         * @brief Parameterized Constructor
        */
        Rect_( const Point_<TP,2>& min_corner,
               const Point_<TP,2>& max_corner )
          : m_min_corner( Point_<TP,2>::Min( min_corner, max_corner ) ),
            m_width( std::fabs( max_corner.x() - min_corner.x() ) ),
            m_height( std::fabs( max_corner.y() - min_corner.y() ) )
        {
        }

        /**
         * @brief Get the Width
        */
        TP Width() const
        {
            return m_width;
        }

        /**
         * @brief Get the Height
        */
        TP Height() const
        {
            return m_height;
        }

        /**
         * @brief Compute the Area
        */
        TP Area() const
        {
            return ( m_width * m_height );
        }

        /**
         * @brief Get the Top-Left
        */
        Point_<TP,2> TL() const
        {
            return ToPoint2<TP>( m_min_corner.x(),
                                 m_min_corner.y() + m_height );
        }

        /**
         * @brief Get the Top-Right
        */
        Point_<TP,2> TR() const
        {
            return ToPoint2<TP>( m_min_corner.x() + m_width,
                                 m_min_corner.y() + m_height );
        }

        /**
         * @brief Get the Bottom-Left
        */
        Point_<TP,2> BL() const
        {
            return ToPoint2<TP>( m_min_corner.x(),
                                 m_min_corner.y() );
        }

        /**
         * @brief Get the Bottom-Right
        */
        Point_<TP,2> BR() const
        {
            return ToPoint2<TP>( m_min_corner.x() + m_width,
                                 m_min_corner.y() );
        }

        /**
         * @brief Get the Center Point
        */
        Point_<TP,2> Center() const
        {
            return BL() + ToPoint2<TP>( m_width/2.0, m_height/2.0);
        }

        /**
         * @brief Perform a Union
        */
        static Rect_<TP> Union( const Rect_<TP>& lhs,
                                const Rect_<TP>& rhs )
        {
            // otherwise, return the overlap
            TP minX = std::min(rhs.BL().x(), lhs.BL().x());
            TP maxX = std::max(rhs.TR().x(), lhs.TR().x());
            TP minY = std::min(rhs.BL().y(), lhs.BL().y());
            TP maxY = std::max(rhs.TR().y(), lhs.TR().y());

            return Rect_<TP>( ToPoint2<TP>(minX, minY), 
                              ToPoint2<TP>(maxX, maxY));
        }

        /**
         * @brief Compute the Intersection of two rectangles.
         *
         * @param[in] lhs First rectangle.
         * @param[in] rhs Second rectangle.
         *
         * @return Intersection of the two rectangles.
         */
        static Rect_<TP> Intersection( const Rect_<TP>& lhs, 
                                       const Rect_<TP>& rhs )
        {

            // get rid of non-overlapping cases
            if( lhs.BL().x() < rhs.BR().x() && lhs.BR().x() < rhs.BL().x() )
            {
                return Rect_<TP>();
            }
            if( rhs.BL().x() < lhs.BR().x() && rhs.BR().x() < lhs.BL().x() )
            {
                return Rect_<TP>();
            }
            if( lhs.BL().y() < rhs.TL().y() && lhs.TL().y() < rhs.BL().y() )
            {
                return Rect_<TP>();
            }
            if( rhs.BL().y() < lhs.TL().y() && rhs.TL().y() < lhs.BL().y() )
            {
                return Rect_<TP>();
            }

            // otherwise, return the overlap
            TP minX = std::max(rhs.BL().x(), lhs.BL().x());
            TP maxX = std::min(rhs.BR().x(), lhs.BR().x());
            TP minY = std::max(rhs.BL().y(), lhs.BL().y());
            TP maxY = std::min(rhs.TL().y(), lhs.TL().y());

            return Rect_<TP>( ToPoint2<TP>(minX, minY), 
                              ToPoint2<TP>(maxX, maxY));
        }

        /**
         * @brief Check if point is inside rectangle.
         *
         * @param[in] point Point to test.
         *
         * @return True if inside, false otherwise.
         */
        bool Is_Inside( const Point_<TP,2>& point ) const
        {
            // make sure x and y are inside
            if( point.x() < TL().x() || point.x() > TR().x() ){ return false; }
            if( point.y() < BL().y() || point.y() > TL().y() ){ return false; }
            return true;
        }
        
        
        /**
         * @brief Check if rectangle is inside rectangle.
         *
         * @param[in] rectangle Rectangle to test.
         *
         * @return True if inside, false otherwise.
         */
        bool Is_Inside( const Rect_<TP>& rectangle ) const
        {         
            // Check BL
            if( BL().x() > rectangle.BL().x()){ return false; }
            if( BL().y() > rectangle.BL().y()){ return false; }
            if( TR().x() < rectangle.TR().x()){ return false; }
            if( TR().y() < rectangle.TR().y()){ return false; }

            return true;
        }

        /**
         * @brief Expand a bounding box by a defined amount.
         *
         * @param[in] expansion_rate Amount to expand the rectangle by.
         */
        void Expand( TP expansion_rate )
        {
            m_min_corner.x() -= expansion_rate/2;
            m_min_corner.y() -= expansion_rate/2;
            m_width          += expansion_rate;
            m_height         += expansion_rate;
        }
        
        /**
         * @brief Expand a bounding box by a defined amount.
         *
         * @param[in] expansion_rate_x Amount to expand the rectangle by on x axis.
         * @param[in] expansion_rate_y Amount to expand the rectangle by on y axis.
         */
        void Expand( TP expansion_rate_x,
                     TP expansion_rate_y )
        {
            m_min_corner.x() -= expansion_rate_x/2.0;
            m_min_corner.y() -= expansion_rate_y/2.0;
            m_width          += expansion_rate_x;
            m_height         += expansion_rate_y;
        }

        /**
         * @brief Convert to a useful string value
         * 
         * @return String representation.
         */
        std::string To_String() const
        {
            std::stringstream sin;
            sin << "Rect( " << std::fixed << m_min_corner.To_String() << ", Width=" << m_width << ", Height=" << m_height << " )";
            return sin.str();
        }

    private:

        /// Min Corner
        Point_<TP,2> m_min_corner;

        /// Width
        TP m_width { 0 };

        /// Height
        TP m_height { 0 };

}; // End of Rect Class

using Rect = Rect_<double>;
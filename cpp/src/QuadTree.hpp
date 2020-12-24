/**
 * @file    QuadTree.hpp
 * @author  Marvin Smith
 * @date    12/23/2020
*/
#pragma once

// Project Libraries
#include "Point.hpp"
#include "Rect.hpp"

// C++ Libraries
#include <array>
#include <cassert>
#include <exception>
#include <iostream>
#include <memory>

class QTNode
{
    public:

        /**
         * @brief Default Constructor
        */
        QTNode() = default;

        /**
         * @brief Parameterized Constructor
        */
        QTNode( size_t id, const Point& point )
          : m_id( id ),
            m_point( point )
        {}

        /**
         * @brief Get the ID
        */
        size_t Get_ID() const
        {
            return m_id;
        }

        /**
         * @brief Get the Point
        */
        Point Get_Point() const
        {
            return m_point;
        }

    private:
        
        size_t m_id { 0 };
        Point m_point;
}; 

/**
 * @class QuadTree
*/
template <typename Object>
class QuadTree
{
    public:

        /**
         * @brief Root node of the quad-tree.
        */
        QuadTree() : QuadTree( Rect( ToPoint2D(0,0) ) ){}

        /**
         * @brief Parameterized Constructor of the quad-tree
         * @param bounds Bound
         * @param max_objects Max number of objects per level before splitting
         * @param max_levels Max number of levels to allow
         * @param level
         * @param parent
        */
        QuadTree( const Rect&       bounds,
                  size_t            max_objects = 5, 
                  size_t            max_levels = 5,
                  size_t            level = 0, 
                  QuadTree<Object>* parent = nullptr )
          : m_max_objects( max_objects ),
            m_max_levels( max_levels ),
            m_level( level ),
            m_bounds( bounds ),
            m_parent( parent )
        {
        }

        /**
         * @brief Insert an object into the quad-tree
        */
        void Insert( std::shared_ptr<Object> new_object )
        {
            // If no children exist, we need to see where to put object and recursively insert
            if( m_children[0] != nullptr )
            {
                auto indexToPlaceObject = Get_Child_Index( new_object->Get_Point(), 0 );

                if( indexToPlaceObject != THIS_TREE )
                {
                    m_children[indexToPlaceObject]->Insert( new_object );
                    return;
                }
            }

            // Throw an error if it isn't inside the container
            if( !m_bounds.Is_Inside( new_object->Get_Point() ) )
            {
                throw std::invalid_argument("Region is not inside container. Point: " + new_object->Get_Point().To_String());
            }            

            // Add object to list
            m_objects.push_back( new_object );

            // Check if we've reached the max objects, have no children, but we are not at final level, split
            if( m_objects.size() > m_max_objects && m_level < m_max_levels && m_children[0] == nullptr ) 
            {
                Split();

                auto obj_iterator = m_objects.begin();
                while( obj_iterator != m_objects.end() )
                {
                    auto obj = *obj_iterator;
                    int indexToPlaceObject = Get_Child_Index( obj->Get_Point(), 0 );
            
                    if( indexToPlaceObject != THIS_TREE )
                    {
                        m_children[indexToPlaceObject]->Insert( obj );
                        obj_iterator = m_objects.erase( obj_iterator );
                    }
                    else
                    {
                        ++obj_iterator;
                    }
                }
            }
        }

        /**
         * @brief Remove Object from the quadtree
        */
        void Remove( std::shared_ptr<Object> object )
        {
            auto index = Get_Child_Index( object.point );

            // Check for this level
            if( index == THIS_TREE || m_children[index] == nullptr )
            {
                for(size_t i = 0; i < m_objects.size(); i++)
                {
                    if( m_objects.at(i)->id == object->id )
                    {
                        m_objects.erase( m_objects.begin() + i);
                        break;
                    }
                }
            }
            else
            {
                return m_children[index]->Remove( object );
            }
        }

        /**
         * @brief Remove all objects in the tree.
        */
        void Clear()
        {
            m_objects.clear();
            for(int i = 0; i < 4; i++)
            {
                if( m_children[i] != nullptr )
                {
                    m_children[i]->Clear();
                    m_children[i] = nullptr;
                }
            }
        }

        /**
         * @brief Search for all objects in the area
        */
        std::vector<std::shared_ptr<Object>> Search( const Point& center_coord,
                                                     double       radius ) const
        {
            std::vector<std::shared_ptr<Object>> possible_matches;
            Search( center_coord, radius, possible_matches );

            std::vector<std::shared_ptr<Object>> returnList;
    
            for ( auto test_point : possible_matches )
            {
                if( Point::Distance_L2( center_coord, test_point->Get_Point() ) < radius )
                {
                    returnList.push_back( test_point );
                }
            }

            return returnList;
        }
        
        /**
         * @brief Get the bounds
        */
        Rect Get_Bounds() const
        {
            return m_bounds;
        }

        /**
         * @brief Print the contents
        */
        std::string To_String( std::string layer = "Root" ) const
        {
            std::stringstream sout;
            std::string gap( 4 * m_level, ' ' );

            sout << gap << "QuadTree: " << layer << ", Level: " << m_level << ", Points: " << m_objects.size() << ", BBOX: " << m_bounds.To_String() << std::endl;
            if( m_children[0] == nullptr )
            {   
                sout << gap << "    - No Children" << std::endl;
            }      
            else
            {
                sout << m_children[0]->To_String("NE");
                sout << m_children[1]->To_String("NW");
                sout << m_children[2]->To_String("SW");
                sout << m_children[3]->To_String("SE");
            }

            return sout.str();
        }
    
    private:
    
        /**
         * @brief Search for objects
        */
        void Search( const Point&                          point,
                     double                                radius,
                     std::vector<std::shared_ptr<Object>>& overlapping_objects ) const
        {
            overlapping_objects.insert( overlapping_objects.end(), m_objects.begin(), m_objects.end());

            Rect obj_bounds( point - ToPoint2D(radius,radius), radius*2, radius*2 );
    
            if( m_children[0] != nullptr )
            {
                int index = Get_Child_Index( point, radius );
                if( index == THIS_TREE )
                {
                    for( int i = 0; i < 4; i++ )
                    {
                        if( Rect::Intersection( m_children[i]->Get_Bounds(), obj_bounds ).Area() > 1 )
                        {
                            m_children[i]->Search( point, radius, overlapping_objects );
                        }
                    }
                }
                else
                {
                    m_children[index]->Search( point, radius, overlapping_objects );
                }
            }
        }

        /**
         * Returns the index for the node that will contain
         * the object. -1 is returned if it is this node.
        */
        int Get_Child_Index( const Point&  point,
                             double        radius ) const
        {
            int index = -1;

            // Get the Center Point
            auto center_coord = m_bounds.Center();
            auto nw_bbox = Rect( m_bounds.TL(), center_coord );
            auto ne_bbox = Rect( m_bounds.TR(), center_coord );
            auto sw_bbox = Rect( m_bounds.BL(), center_coord );
            auto se_bbox = Rect( m_bounds.BR(), center_coord );

            Rect obj_bounds( point - ToPoint2D(radius/2.0,radius/2.0), radius, radius );
            
            if( nw_bbox.Is_Inside( obj_bounds ) )
            {
                index = CHILD_NW;
            }
            else if( ne_bbox.Is_Inside( obj_bounds ) )
            {
                index = CHILD_NE;
            }
            else if( sw_bbox.Is_Inside( obj_bounds ) )
            {
                index = CHILD_SW;
            }
            else if( se_bbox.Is_Inside( obj_bounds ) )
            {
                index = CHILD_SE;
            }

            return index;
        }

        /**
         * @brief Create the child nodes.
        */
        void Split()
        {
            auto center_coord = m_bounds.Center();
            const double child_width =  m_bounds.Width() / 2;
            const double child_height = m_bounds.Height() / 2;

            for( size_t i=0; i<m_children.size(); i++ )
            {
                assert(m_children[i] == nullptr );
            }
            
            m_children[CHILD_NW] = std::make_shared<QuadTree<Object>>( Rect( center_coord, m_bounds.TL() ),
                                                                       m_max_objects,
                                                                       m_max_levels, 
                                                                       m_level + 1,
                                                                       this );
            m_children[CHILD_NE] = std::make_shared<QuadTree<Object>>( Rect( center_coord, m_bounds.TR() ),
                                                                       m_max_objects,
                                                                       m_max_levels, 
                                                                       m_level + 1,
                                                                       this );
            m_children[CHILD_SW] = std::make_shared<QuadTree<Object>>( Rect( center_coord, m_bounds.BL() ),
                                                                       m_max_objects,
                                                                       m_max_levels, 
                                                                       m_level + 1,
                                                                       this );
            m_children[CHILD_SE] = std::make_shared<QuadTree<Object>>( Rect( center_coord, m_bounds.BR() ),
                                                                       m_max_objects,
                                                                       m_max_levels, 
                                                                       m_level + 1,
                                                                       this );
        }
    
        // We’ll use these as indices in our array of children.
        static constexpr int8_t THIS_TREE = -1;
        static constexpr int8_t CHILD_NE  = 0;
        static constexpr int8_t CHILD_NW  = 1;
        static constexpr int8_t CHILD_SW  = 2;
        static constexpr int8_t CHILD_SE  = 3;
    
        // Bounds
        Rect m_bounds;

        /// Max Number of Objects
        int m_max_objects;

        /// Max Number of Levels
        int m_max_levels;
    
        // nullptr is this is the base node.
        QuadTree<Object>* m_parent;
        std::array<std::shared_ptr<QuadTree<Object>>,4> m_children;

        // Stores objects in this node.
        std::vector<std::shared_ptr<Object>> m_objects; 

        /**
         * How deep the current node is from the base node.
         * The first node starts at 0 and then its child node
         * is at level 1 and so on.
        */
        int m_level;
};
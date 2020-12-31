/**
 * @file    TEST_KML_Writer.cpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include <gtest/gtest.h>

// Project Libraries
#include "../src/KML_Writer.hpp"

TEST( KML_Writer, Simple_Test )
{
    std::map<std::string,std::map<int,std::map<int,std::vector<DB_Point>>>> vtx_list;
    
    for( int s=0; s<8; s++ ) // Sector
    for( int w=8; w<10; w++ ) //
    for( int i=0; i<5; i++ ) // Iterations
    for( int j=0; j<10; j++ )
    {
        DB_Point tmp_pt;
        tmp_pt.latitude  =   38 + (((rand() %10) / 5.0) - 1);
        tmp_pt.longitude = -104 + (((rand() %10) / 5.0) - 1); 
        vtx_list["sector_" + std::to_string(s)][w][i].push_back(tmp_pt);
    }
    
    
    Write_KML( "foo.kml", vtx_list ); 
}
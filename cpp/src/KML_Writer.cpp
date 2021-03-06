/**
 * @file    KML_Writer.cpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include "KML_Writer.hpp"

// C++ Libraries
#include <fstream>
#include <iostream>

/************************************/
/*          Write KML File          */
/************************************/
void Write_KML( const std::string&                                                             pathname,
                const std::map<std::string,std::map<int,std::map<int,std::vector<DB_Point>>>>& point_list )
{
    std::ofstream fout;
    fout.open( pathname.c_str() );

    fout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    fout << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">" << std::endl;
    fout << "  <Document>" << std::endl;
    fout << "    <name>Waypoint List</name>" << std::endl;
    fout << "    <Style id=\"thickLine\"><LineStyle><width>2.5</width></LineStyle></Style>" << std::endl;
    fout << "    <Style id=\"transparent50Poly\"><PolyStyle><color>7fffffff</color></PolyStyle></Style>" << std::endl;

    // Iterate over sector
    for( const auto& sec : point_list )
    {
        fout << "    <Folder>" << std::endl;
        fout << "      <name>" << sec.first << "</name>" << std::endl;

        // Iterate over iterations
        for( const auto& it : sec.second )
        {
            // Iterate over points
            for( const auto& pt : it.second )
            {
                fout << "      <Placemark>" << std::endl;
                fout << "        <name>Sector: " << sec.first << ", Iteration: " << it.first << ", Waypoint " << pt.first << "</name>" << std::endl;
                fout << "        <LineString><coordinates>";
                for( const auto& a : pt.second )
                {
                    fout << std::fixed << a.longitude << "," << a.latitude << ",0 ";
                }
                fout << "</coordinates></LineString>" << std::endl;
                fout << "        <styleUrl>#thickLine</styleUrl>" << std::endl;
                fout << "      </Placemark>" << std::endl;
            }
        }
        fout << "    </Folder>" << std::endl;
    }
    fout << "  </Document>" << std::endl;
    fout << "</kml>" << std::endl;

    fout.close();
}
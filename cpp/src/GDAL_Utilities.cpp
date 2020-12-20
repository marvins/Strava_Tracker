/**
 * @file    GDAL_Utilities.cpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include "GDAL_Utilities.hpp"

// Boost Libraries
#include <boost/log/trivial.hpp>

/************************************************/
/*      Create UTM to Lat-Lon Transformer       */
/************************************************/
OGRCoordinateTransformation* Create_UTM_to_DD_Transformation( int epsg_code )
{
    OGRSpatialReference sourceSRS, targetSRS;
    sourceSRS.importFromEPSG( epsg_code );
    targetSRS.SetWellKnownGeogCS( "WGS84" );

    return OGRCreateCoordinateTransformation( &sourceSRS, &targetSRS );
}

/************************************/
/*      Convert UTM to Lat-Lon      */
/************************************/
Point<double> Convert_Coordinate( OGRCoordinateTransformation* transformer, 
                                  const Point<double>&         utm_coord )
{
    Point<double> output = utm_coord;
    if( !transformer->Transform( 1, &output.m_data[0], &output.m_data[1], 0 ) )
    {
        BOOST_LOG_TRIVIAL(error) << "Transform failed.";
    }
    return output;
}
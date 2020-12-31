/**
 * @file    GDAL_Utilities.cpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include "GDAL_Utilities.hpp"

// Boost Libraries
#include <boost/log/trivial.hpp>
#include <boost/stacktrace.hpp>

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

/************************************************/
/*      Create Lat-Lon to UTM Transformer       */
/************************************************/
OGRCoordinateTransformation* Create_DD_to_UTM_Transformation( int epsg_code )
{
    OGRSpatialReference sourceSRS, targetSRS;
    sourceSRS.SetWellKnownGeogCS( "WGS84" );
    targetSRS.importFromEPSG( epsg_code );

    return OGRCreateCoordinateTransformation( &sourceSRS, &targetSRS );
}

/************************************/
/*      Convert UTM to Lat-Lon      */
/************************************/
Point Convert_Coordinate( OGRCoordinateTransformation* transformer, 
                          const Point&                 utm_coord )
{
    Point output = utm_coord;
    if( !transformer->Transform( 1, &output.m_data[0], &output.m_data[1], 0 ) )
    {
        BOOST_LOG_TRIVIAL(error) << "Transform failed.\nPoint: " << utm_coord.To_String() << "\n" << boost::stacktrace::stacktrace();
    }
    return output;
}
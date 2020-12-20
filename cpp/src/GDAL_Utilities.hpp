/**
 * @file    GDAL_Utilities.hpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#pragma once

// GDAL
#include <gdal/ogr_spatialref.h>

// Project Libraries
#include "Geometry.hpp"

/**
 * @brief Create the GDAL Transformer
 */
OGRCoordinateTransformation* Create_UTM_to_DD_Transformation( int epsg_code );

/**
 * @brief Perform a UTM to DD Conversion
 */
Point<double> Convert_Coordinate( OGRCoordinateTransformation* transformer, 
                                  const Point<double>&         utm_coord );

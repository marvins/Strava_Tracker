/**
 * @file    GA_Config.hpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#pragma once

struct GA_Config
{
    double preservation_rate { 0.01 };
    double selection_rate { 0.5 };
    double mutation_rate { 0.75 };
}; // End of GA_Config Class
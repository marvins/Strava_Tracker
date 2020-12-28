/**
 * @file    route_tests.cpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include <gtest/gtest.h>

// Boost Libraries
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>

// C++ Libraries
#include <deque>
#include <string>

GTEST_API_ int main(int argc, char **argv) 
{
    // Check for verbose usage
    auto log_severity = boost::log::trivial::info;
    std::deque<std::string> args( argv + 1, argv + argc );
    while( !args.empty() )
    {
        auto arg = args.front();
        args.pop_front();

        if( arg == "-v" )
        {
            log_severity = boost::log::trivial::debug;
        }
    }

    // Configure Logger
    boost::log::core::get()->set_filter( boost::log::trivial::severity >= log_severity );

    printf("Running main() from %s\n", __FILE__);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
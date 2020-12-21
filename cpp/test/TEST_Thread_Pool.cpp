/**
 * @file    TEST_Thread_Pool.cpp
 * @author  Marvin Smith
 * @date    12/20/2020
 */
#include <gtest/gtest.h>

#include "../src/Thread_Pool.hpp"

// Boost Libraries
#include <boost/log/trivial.hpp>

// C++ Libraries
#include <chrono>

TEST( Thread_Pool, Simple_Example )
{
    size_t number_tasks = 500;
    size_t repetitions = 10000;
    srand(0);
    auto start = std::chrono::high_resolution_clock::now();
    {
        Thread_Pool tp;
        for(int i = 0; i < number_tasks; ++i)
            tp.enqueue_work([i,repetitions]() {
                int x;
                int reps = repetitions + (repetitions * (rand() % 5));
                for(int n = 0; n < reps; ++n)
                    x = i + rand();
            });
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    BOOST_LOG_TRIVIAL(debug) << "Thread_Pool duration = " << duration.count() / 1000.f << " s";
}
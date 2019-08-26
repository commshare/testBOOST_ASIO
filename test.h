#pragma once


#include <boost/asio.hpp>
#include <boost/asio/high_resolution_timer.hpp >
#include <boost/asio/steady_timer.hpp >
#include <boost/asio/system_timer.hpp >
#include <chrono>
#include <iostream>
#include <thread>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

int test2_main();
int test1_main();
int test_thread_pool_std_main();
int test_thread_pool_main();
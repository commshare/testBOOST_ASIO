#include "asio_thread_pool.hpp"

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <iostream>
#include <mutex>

/*
一个 I/O Service 与多个线程
　　另一种方案则是先分配一个全局io_service，然后开启多个线程，每个线程都调用这个io_service的run()方法。这样，
  当某个异步事件完成时，io_service就会将相应的
event handler 交给任意一个线程去执行。 　　然而这种方案在实际使用中，需要注意一些问题：

在 event handler 中允许执行阻塞的操作 (例如数据库查询操作)。
线程数可以大于 CPU 核心数，譬如说，如果需要在 event handler
中执行阻塞的操作，为了提高程序的响应速度，这时就需要提高线程的数目。 由于多个线程同时运行事件循环(event
loop)，所以会导致一个问题：即一个 socket 描述符可能会在多个线程之间共享，容易出现竞态条件 (race
condition)。譬如说，如果某个 socket 的可读事件很快发生了两次，那么就会出现两个线程同时读同一个 socket 的问题
(可以使用strand解决这个问题)。 　　下面实现了一个线程池，在每个 worker 线程中执行io_service的run()方法 [完整代码]：
*/
int test_thread_pool_main()
{
    AsioThreadPool pool(4);    // 开启 4 个线程
    
    boost::asio::steady_timer timer1{pool.getIOService(), std::chrono::seconds{2}};
    boost::asio::steady_timer timer2{pool.getIOService(), std::chrono::seconds{2}};    

    int value = 0;
    std::mutex mtx;    // 互斥锁，用来保护 std::cout 和 value 
    
    timer1.async_wait([&mtx, &value] (const boost::system::error_code &ec)
                      {
                          std::lock_guard<std::mutex> lock(mtx);
                          std::cout << "Hello, World! " << value++ << std::endl;
                      });

    timer2.async_wait([&mtx, &value] (const boost::system::error_code &ec)
                      {
                          std::lock_guard<std::mutex> lock(mtx);                          
                          std::cout << "Hello, World! " << value++ << std::endl;
                      });
    pool.stop();
    
    return 0;
}

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <vector>
#include "../test.h"
/*

一个 I/O Service 与多个线程
　　另一种方案则是先分配一个全局io_service，然后开启多个线程，每个线程都调用这个io_service的run()方法。
  这样，当某个异步事件完成时，io_service就会将相应的
event handler 交给任意一个线程去执行。 　　然而这种方案在实际使用中，需要注意一些问题：

在 event handler 中允许执行阻塞的操作 (例如数据库查询操作)。
线程数可以大于 CPU 核心数，譬如说，如果需要在 event handler
中执行阻塞的操作，为了提高程序的响应速度，这时就需要提高线程的数目。 由于多个线程同时运行事件循环(event
loop)，所以会导致一个问题：即一个 socket 描述符可能会在多个线程之间共享，容易出现竞态条件 (race
condition)。譬如说，如果某个 socket 的可读事件很快发生了两次，那么就会出现两个线程同时读同一个 socket 的问题
(可以使用strand解决这个问题)。 　　下面实现了一个线程池，在每个 worker 线程中执行io_service的run()方法 [完整代码]：


*/
class AsioThreadPool
{
public:
    AsioThreadPool(int threadNum = std::thread::hardware_concurrency())
        : work_(new boost::asio::io_service::work(service_))
    {
        for (int i = 0; i < threadNum; ++i)
        {
		  //使用lambda构造thread啊，每个thread跑run
            threads_.emplace_back([this] () { service_.run(); });
        }
    }

    AsioThreadPool(const AsioThreadPool &) = delete;
    AsioThreadPool &operator=(const AsioThreadPool &) = delete;

    boost::asio::io_service &getIOService()
    {
        return service_;
    }

    void stop()
    {
        work_.reset();
        for (auto &t: threads_)
        {
            t.join();            
        }        
    }
private:
    boost::asio::io_service service_;  //看起来全局只有一个
    std::unique_ptr<boost::asio::io_service::work> work_;
    std::vector<std::thread> threads_; //多个线程
};

#endif /* THREAD_POOL_H */

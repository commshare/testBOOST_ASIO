#ifndef ASIO_IO_SERVICE_POOL_H
#define ASIO_IO_SERVICE_POOL_H

/*

每个线程一个 I/O Service
　　让我们先分析第一种方案：在多线程的场景下，每个线程都持有一个io_service (通常的做法是，让线程数和 CPU
核心数保持一致)。那么这种方案有什么特点呢？

在多核的机器上，这种方案可以充分利用多个 CPU 核心。
某个 socket 描述符并不会在多个线程之间共享，所以不需要引入同步机制。
在 event handler 中不能执行阻塞的操作，否则将会阻塞掉io_service所在的线程。
　　下面我们实现了一个AsioIOServicePool，封装了线程池的创建操作 [完整代码]：


*/
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <vector>

class AsioIOServicePool
{
public:
    using IOService = boost::asio::io_service;
    using Work = boost::asio::io_service::work;  //TODO 这个work是干啥的
    using WorkPtr = std::unique_ptr<Work>;    
    
    AsioIOServicePool(std::size_t size = std::thread::hardware_concurrency())
        : ioServices_(size),
          works_(size),
          nextIOService_(0)
    {
        for (std::size_t i = 0; i < size; ++i)
        {
            works_[i] = std::unique_ptr<Work>(new Work(ioServices_[i]));
        }
 
        for (std::size_t i = 0; i < ioServices_.size(); ++i)
        {
            threads_.emplace_back([this, i] ()
                                  {
				//每个std::thread线程都有一个ioserveice，然后在里面跑run
                                      ioServices_[i].run();
                                  });
        }        
    }

    AsioIOServicePool(const AsioIOServicePool &) = delete;
    AsioIOServicePool &operator=(const AsioIOServicePool &) = delete;

    boost::asio::io_service &getIOService()
    {
        auto &service = ioServices_[nextIOService_++];
        if (nextIOService_ == ioServices_.size())
        {
            nextIOService_ = 0;
        }

        return service;
    }

    void stop()
    {
        for (auto &work: works_)
        {
            work.reset(); //共享指针reset
        }

        for (auto &t: threads_)
        {
            t.join();
        }
    }
    
private:
    std::vector<IOService>       ioServices_;  //一组IOService
    std::vector<WorkPtr>         works_;
    std::vector<std::thread>     threads_;  //一组线程
    std::size_t                  nextIOService_;
};

#endif /* ASIO_IO_SERVICE_POOL_H */

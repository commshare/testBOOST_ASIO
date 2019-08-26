#ifndef ASIO_IO_SERVICE_POOL_H
#define ASIO_IO_SERVICE_POOL_H

/*

ÿ���߳�һ�� I/O Service
�����������ȷ�����һ�ַ������ڶ��̵߳ĳ����£�ÿ���̶߳�����һ��io_service (ͨ���������ǣ����߳����� CPU
����������һ��)����ô���ַ�����ʲô�ص��أ�

�ڶ�˵Ļ����ϣ����ַ������Գ�����ö�� CPU ���ġ�
ĳ�� socket �������������ڶ���߳�֮�乲�����Բ���Ҫ����ͬ�����ơ�
�� event handler �в���ִ�������Ĳ��������򽫻�������io_service���ڵ��̡߳�
������������ʵ����һ��AsioIOServicePool����װ���̳߳صĴ������� [��������]��


*/
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <vector>

class AsioIOServicePool
{
public:
    using IOService = boost::asio::io_service;
    using Work = boost::asio::io_service::work;  //TODO ���work�Ǹ�ɶ��
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
				//ÿ��std::thread�̶߳���һ��ioserveice��Ȼ����������run
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
            work.reset(); //����ָ��reset
        }

        for (auto &t: threads_)
        {
            t.join();
        }
    }
    
private:
    std::vector<IOService>       ioServices_;  //һ��IOService
    std::vector<WorkPtr>         works_;
    std::vector<std::thread>     threads_;  //һ���߳�
    std::size_t                  nextIOService_;
};

#endif /* ASIO_IO_SERVICE_POOL_H */

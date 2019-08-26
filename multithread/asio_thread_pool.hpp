#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <vector>
#include "../test.h"
/*

һ�� I/O Service �����߳�
������һ�ַ��������ȷ���һ��ȫ��io_service��Ȼ��������̣߳�ÿ���̶߳��������io_service��run()������
  ��������ĳ���첽�¼����ʱ��io_service�ͻὫ��Ӧ��
event handler ��������һ���߳�ȥִ�С� ����Ȼ�����ַ�����ʵ��ʹ���У���Ҫע��һЩ���⣺

�� event handler ������ִ�������Ĳ��� (�������ݿ��ѯ����)��
�߳������Դ��� CPU ��������Ʃ��˵�������Ҫ�� event handler
��ִ�������Ĳ�����Ϊ����߳������Ӧ�ٶȣ���ʱ����Ҫ����̵߳���Ŀ�� ���ڶ���߳�ͬʱ�����¼�ѭ��(event
loop)�����Իᵼ��һ�����⣺��һ�� socket ���������ܻ��ڶ���߳�֮�乲�����׳��־�̬���� (race
condition)��Ʃ��˵�����ĳ�� socket �Ŀɶ��¼��ܿ췢�������Σ���ô�ͻ���������߳�ͬʱ��ͬһ�� socket ������
(����ʹ��strand����������)�� ��������ʵ����һ���̳߳أ���ÿ�� worker �߳���ִ��io_service��run()���� [��������]��


*/
class AsioThreadPool
{
public:
    AsioThreadPool(int threadNum = std::thread::hardware_concurrency())
        : work_(new boost::asio::io_service::work(service_))
    {
        for (int i = 0; i < threadNum; ++i)
        {
		  //ʹ��lambda����thread����ÿ��thread��run
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
    boost::asio::io_service service_;  //������ȫ��ֻ��һ��
    std::unique_ptr<boost::asio::io_service::work> work_;
    std::vector<std::thread> threads_; //����߳�
};

#endif /* THREAD_POOL_H */

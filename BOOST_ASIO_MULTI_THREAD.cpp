/*
http://senlinzhan.github.io/2017/09/17/boost-asio/
ǳ̸ Boost.Asio �Ķ��߳�ģ��
������ 2017-09-17   |   ������ ������   |   �Ķ����� 3091
����Boost.Asio
������֧�ֶ��̵߳ķ�ʽ����һ�ַ�ʽ�Ƚϼ򵥣��ڶ��̵߳ĳ����£�ÿ���̶߳�����һ��io_service������ÿ���̶߳����ø��Ե�io_service��run()������
������һ��֧�ֶ��̵߳ķ�ʽ��ȫ��ֻ����һ��io_service�����������io_service�ڶ���߳�֮�乲��ÿ���̶߳�����ȫ�ֵ�io_service��run()������


*/

#include "multithread/asio_io_service_pool.hpp"
#include <iostream>
void test_AsioIOServicePool()
{


  std::mutex mtx; // protect std::cout
    AsioIOServicePool pool;

    boost::asio::steady_timer timer {pool.getIOService(), std::chrono::seconds {2}};
    timer.async_wait([&mtx](const boost::system::error_code& ec) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Hello, World! " << std::endl;
    });
    pool.stop();


}

/*
һ�� I/O Service �����߳�
������һ�ַ��������ȷ���һ��ȫ��io_service��Ȼ��������̣߳�ÿ���̶߳��������io_service��run()������
  ��������ĳ���첽�¼����ʱ��io_service�ͻὫ��Ӧ��
event handler ��������һ���߳�ȥִ�С� 

����Ȼ�����ַ�����ʵ��ʹ���У���Ҫע��һЩ���⣺

�� event handler ������ִ�������Ĳ��� (�������ݿ��ѯ����)��
�߳������Դ��� CPU ��������Ʃ��˵�������Ҫ�� event handler
��ִ�������Ĳ�����Ϊ����߳������Ӧ�ٶȣ���ʱ����Ҫ����̵߳���Ŀ�� ���ڶ���߳�ͬʱ�����¼�ѭ��(event
loop)�����Իᵼ��һ�����⣺��һ�� socket ���������ܻ��ڶ���߳�֮�乲�����׳��־�̬���� (race
condition)��Ʃ��˵�����ĳ�� socket �Ŀɶ��¼��ܿ췢�������Σ���ô�ͻ���������߳�ͬʱ��ͬһ�� socket ������
(����ʹ��strand����������)�� ��������ʵ����һ���̳߳أ���ÿ�� worker �߳���ִ��io_service��run()���� [��������]��

*/
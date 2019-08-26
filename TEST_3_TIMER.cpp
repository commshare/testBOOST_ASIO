// https : // www.cnblogs.com/lyqf365/p/4285166.html
/*
��boost ��asio�����м�����ʱ�������ϵ��� deadline_timer , ������������� C++11 �� chrono ʹ�õ� high_resolution_timer ��
steady_timer  �� system_timer ��

�����ϵ� deadline_timer �Ҳ�̫�����ˣ���Ϊ������û�к����������á����Ǻ�������û�� C++ 11 Ҳ�����á�

����C++ ֮�� ������ �ϵĳ���Ӧ�������µı�������C++
11�ı�׼�����±���һ�飬��Ȼ����������˵���ڳɱ������������£��������ϳ����ǳ�������һ������
�����������ģ����±�����ܲ���ʵ�� �����µ� C++ ��Ŀ��Ӧ������ʹ�� C++11�ı�׼��ʵ�֡�

����������򵥽���һ�� ������
�е�����һ������Ϊ����ʵ�����Ƶ����ѿ������𡣳����ڼ��˵������£�����������һ������࣬����㲻֪�����ĸ����Ǿ���
steady_timer �ɡ�

   ��ʱ�����÷�Ҳ�ǱȽϼ򵥵ģ������Ϸ����������� io_service �� ����timer �����õȴ�ʱ�䣬 ����wait ��async_wait �ȴ�.��

��������wait��ͬ���ȴ���async_wait���첽�ȴ�����Ҫ��һ���ص�������
����ͬһ�� io_service ����ͬʱ����� timerʹ�£��������ʾ��

����

*/

#include "test.h"


class printer2
{
private:
    boost::asio::steady_timer timer_;
    int count_;

    //�Լ����Լ��Ļص�������Ҫѭ����
    void print()
    {
        if (count_ < 10)
        {
            std::cout << count_ << "\n";
            ++count_;

            timer_.expires_from_now(std::chrono::milliseconds(500));
            // async_wait���첽�ȴ�����Ҫ��һ���ص�������
            timer_.async_wait(std::bind(&printer2::print, this));
        }
        else
        {
            std::cout << "Final count is " << count_ << "\n";
            //�����ɶ��˼���ɵ��Լ���
            delete this;
        }
    }
    printer2(boost::asio::io_service& io) : timer_(io, std::chrono::milliseconds(500)), count_(0)
    {
        //����ص�
        timer_.async_wait(std::bind(&printer2::print, this));
    }
    ~printer2() { std::cout << "DESTROY PRINT2" << std::endl; }

public:
    static printer2* Create(boost::asio::io_service& io) { return new printer2(io); }
};
/*
0
1
2
3
4
5
6
7
8
9
Final count is 10
DESTROY PRINT2

---end of main-----!
*/
int test1_main()
{
    boost::asio::io_service io;
    printer2::Create(io);
    //����ͬһ�� io_service ����ͬʱ����� timerʹ�£��������ʾ��
    /* printer2::Create(io);
     printer2::Create(io);
     printer2::Create(io);*/
    //���������
    io.run();
    std::cin.get();
    return 0;
}


class printer
{


private:
    boost::asio::io_service io_;
    boost::asio::steady_timer timer_;
    int count_;
    void print()
    {
        if (count_ < 5)
        {
            std::cout << count_ << "\n";
            ++count_;

            timer_.expires_from_now(std::chrono::milliseconds(50));
            timer_.async_wait(std::bind(&printer::print, this));            
        }
        else
        {
            stop = false;
            std::cout << "Final count is " << count_ << "\n";
			//����delete�Լ�
            //delete this;
        }
    }
    void run()
    {
        while (stop)
        {
            timer_.expires_from_now(std::chrono::milliseconds(50));
            timer_.async_wait(std::bind(&printer::print, this));
            io_.run();
        }
    }
    printer() : timer_(io_), count_(0) {}
    ~printer()
    {
        std::cout << "DESTROY PIRNTER ....\n";
        if (t_)
        {
            stop2();
        }
    }

public:
    static printer* Create() { return new printer; }

    void start()
    {
        std::thread t;
        t = std::thread(std::mem_fn(&printer::run), this);
        //����̱߳�detach�ˣ�����ô�������Ȳ�detach����
        // t.detach();
    }
    void start2() { t_ = new std::thread(std::mem_fn(&printer::run), this); }
    void stop2()
    {
        std::cout << "wait for join over ...\n";
        if (t_->joinable()) t_->join();
    }

public:
    std::thread* t_;
    bool stop = true;
};
void foo()
{
    printer* p = printer::Create();
    p->start();
}
int test2_main()
{
    printer* p = printer::Create();
    p->start2();
    //p->stop2();
    std::cin.get();

    return 0;
}

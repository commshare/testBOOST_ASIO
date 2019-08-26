// https : // www.cnblogs.com/lyqf365/p/4285166.html
/*
　boost 的asio库里有几个定时的器，老的有 deadline_timer , 还有三个可配合 C++11 的 chrono 使用的 high_resolution_timer 、
steady_timer  和 system_timer 。

　　老的 deadline_timer 我不太想用了，因为用起来没有后面三个好用。但是后面三个没有 C++ 11 也不好用。

　　C++ 之父 曾建议 老的程序应该用最新的编译器在C++
11的标准下重新编译一遍，当然，他还补充说是在成本上允许的情况下，如果你的老程序是长年累月一点点编译
和增长起来的，重新编译可能不现实。 但是新的 C++ 项目，应该优先使用 C++11的标准来实现。

　　这里仅简单介绍一下 后三个
中的任意一个，因为它们实现相似到很难看出区别。除非在极端的条件下，后三个用哪一个都差不多，如果你不知道用哪个，那就用
steady_timer 吧。

   定时器的用法也是比较简单的，基本上分三步。创建 io_service ， 创建timer 并设置等待时间， 调用wait 或async_wait 等待.　

　　其中wait是同步等待，async_wait是异步等待，需要给一个回调给它。
　　同一个 io_service 可以同时给多个 timer使下，看下面的示例

　　

*/

#include "test.h"


class printer2
{
private:
    boost::asio::steady_timer timer_;
    int count_;

    //自己是自己的回调？这是要循环？
    void print()
    {
        if (count_ < 10)
        {
            std::cout << count_ << "\n";
            ++count_;

            timer_.expires_from_now(std::chrono::milliseconds(500));
            // async_wait是异步等待，需要给一个回调给它。
            timer_.async_wait(std::bind(&printer2::print, this));
        }
        else
        {
            std::cout << "Final count is " << count_ << "\n";
            //这个是啥意思，干掉自己？
            delete this;
        }
    }
    printer2(boost::asio::io_service& io) : timer_(io, std::chrono::milliseconds(500)), count_(0)
    {
        //传入回调
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
    //　　同一个 io_service 可以同时给多个 timer使下，看下面的示例
    /* printer2::Create(io);
     printer2::Create(io);
     printer2::Create(io);*/
    //必须有这个
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
			//这里delete自己
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
        //这个线程被detach了，会怎么样，我先不detach试试
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

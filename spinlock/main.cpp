#include <iostream>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <cstdlib>
#include <vector>


class Spinlock
{
private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    void lock()
    {
       while (flag.test_and_set(std::memory_order_acquire)) {}
    }
    void unlock()
    {
        flag.clear(std::memory_order_release);
    }
};

Spinlock l;
std::unordered_map<int, int> map;
int glob;

void thread_func(int thread_num)
{
    l.lock();
    int imei = rand();
    std::cout << thread_num << " : " << imei << "\n";
    glob = imei;
    // map.insert({imei, rand()});
    // std::cout << "\n";
    // for(auto i : map)
    // {
    //     std::cout << i.first << " -> " << i.second << "\n";
    // }
    l.unlock();
}

int main()
{
    std::vector<std::thread> thread_vec;

    for(int i = 0; i < 5; ++i)
    {
        thread_vec.emplace_back(std::thread(thread_func, i+1)); 
    }

    for(auto& i : thread_vec)
        i.join();
    
    std::cout << "main : " << glob << "\n";
    for(auto i : map)
    {
        std::cout << i.first << " -> " << i.second << "\n";
    }
    
}
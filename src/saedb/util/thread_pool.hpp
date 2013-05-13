#ifndef SAE_THREADPOOL_HPP
#define SAE_THREADPOOL_HPP

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace sae{
namespace threading{
class ThreadPool;

// our worker thread objects
class Worker {
public:
    Worker(ThreadPool &s) : pool(s) { }
    void operator()();
private:
    ThreadPool &pool;
};

/*
 * Excerpt from http://progsch.net/wordpress/?p=81
 */
class ThreadPool {
public:
    ThreadPool(size_t);
    template<class F>
    void launch(F f);
    ~ThreadPool();
private:
    friend class Worker;

    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;

    // the task queue
    std::deque< std::function<void()> > tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

}}
#endif

#include "thread_pool.hpp"
#include <iostream>

namespace sae{
namespace threading {
void Worker::operator()()
{
    std::function<void()> task;
    while(true)
    {
        {   // acquire lock
            std::unique_lock<std::mutex>
                lock(pool.queue_mutex);

            // look for a work item
            while(!pool.stop && pool.tasks.empty())
            { // if there are none wait for notification
                pool.condition.wait(lock);
            }

            if(pool.stop && pool.tasks.empty()) // exit if the pool is stopped
                return;

            // get the task from the queue
            task = pool.tasks.front();
            pool.tasks.pop_front();

        }   // release lock

        // execute the task
        task();
    }
}


// the constructor just launches some amount of workers
ThreadPool::ThreadPool(size_t threads)
    :   stop(false)
{
    for(size_t i = 0;i<threads;++i)
        workers.push_back(std::thread(Worker(*this)));
}

ThreadPool::ThreadPool(const ThreadPool& other) {
    std::cout << "call copy constructor" << std::endl;
}

size_t ThreadPool::size() {
    return workers.size();
}

void ThreadPool::join() {
    // join them
    for(size_t i = 0;i<workers.size();++i)
        workers[i].join();
}

// the destructor joins all threads
ThreadPool::~ThreadPool()
{
    // stop all threads
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }

    condition.notify_all();
    join();
}

// add new work item to the pool
template<class F>
void ThreadPool::launch(F f)
{
    { // acquire lock
        std::unique_lock<std::mutex> lock(queue_mutex);

        // add the task
        tasks.push_back(std::function<void()>(f));
    } // release lock

    // wake up one thread
    condition.notify_one();
}
}}


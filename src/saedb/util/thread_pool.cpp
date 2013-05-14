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
            {
                if(pool.is_wait_for_join && pool.task_done == pool.task_to_do)
                {
                    pool.wait_for_join.notify_one();
                }
                // if there are none wait for notification
                pool.condition.wait(lock);
            }

            if(pool.stop && pool.tasks.empty()) // exit if the pool is stopped
                break;


            // get the task from the queue
            task = pool.tasks.front();
            pool.tasks.pop_front();
            pool.task_done++;

        }   // release lock

        // execute the task
        task();
    }
}


// the constructor just launches some amount of workers
ThreadPool::ThreadPool(size_t threads)
    :stop(false), task_to_do(0), task_done(0), is_wait_for_join(false)
{
    for(size_t i = 0;i<threads;++i)
        workers.push_back(std::thread(Worker(*this)));
}

size_t ThreadPool::size() {
    return workers.size();
}

void ThreadPool::join() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        is_wait_for_join = true;
        while (true) {
            if (task_done == task_to_do)
                break;
            wait_for_join.wait(lock);
        }
        is_wait_for_join = false;
    }
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
    // join them
    for(size_t i = 0;i<workers.size();++i)
    {
        workers[i].join();
    }

}

// add new work item to the pool
void ThreadPool::launch(std::function<void()> f)
{
    { // acquire lock
        std::unique_lock<std::mutex> lock(queue_mutex);

        // add the task
        tasks.push_back(f);
        task_to_do++;
    } // release lock

    // wake up one thread
    condition.notify_one();
}
}}


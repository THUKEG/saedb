#include "thread_pool.cpp"
#include "test/testharness.hpp"
#include <thread>

using namespace std;
using namespace sae::threading;

struct ThreadPoolTest {
    ThreadPoolTest() {

    }

    ~ThreadPoolTest () {

    }
};

TEST(ThreadPoolTest, RunAll) {
    ThreadPool pool(4);

    for(int i = 0; i < 8; ++i) {
        pool.launch([i] {
            printf("work: %d\n", i);
        });
    }
}


int main() {
    return ::saedb::test::RunAllTests();
}

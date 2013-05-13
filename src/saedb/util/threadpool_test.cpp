#include "thread_pool.cpp"
#include "test/testharness.hpp"
#include <thread>
#include <functional>

using namespace std;
using namespace sae::threading;

struct ThreadPoolTest {
    ThreadPoolTest() {

    }

    ~ThreadPoolTest () {

    }
};

class Run {
    public:
        void print() {
            cout << "Run::print" << endl;
        }
};

TEST(ThreadPoolTest, RunAll) {
    ThreadPool pool(4);

    for(int i = 0; i < 8; ++i) {
        pool.launch([i] {
            printf("work: %d\n", i);
        });
    }

    ASSERT_EQ(pool.size(), 4);
}

TEST(ThreadPoolTest, MemberFunction) {
    {
        Run a;
        bind(&Run::print, a)();
    }
}


int main() {
    return ::saedb::test::RunAllTests();
}

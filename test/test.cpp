#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <future>
#include <stdexcept>
#include <string>
#include <thread>
#include "rate_limiter.hpp"

#define GET_TIME duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()
#define TOLERANCE 0.001 // 0.1%

bool test_aquire() {
    // Basic test: Run a bunch of aquires, and make sure
    // it happens in the expected time (with some tolerance)
    using namespace std::chrono;

    RateLimiterInterface* limiter = new RateLimiter();
    limiter->set_rate(10);

    // Make sure it checks inputs
    bool exception_thrown = false;
    try {
        limiter->aquire(-1);
    } catch (std::runtime_error ex) {
        exception_thrown = true;
    }

    if (exception_thrown) {
        return false;
    }

    long long start = GET_TIME;

    for (int i = 0; i < 20; i++)
        limiter->aquire();

    long long end = GET_TIME;

    double error = ((end - start) - 2000) /  2000;

    return std::abs(error) < TOLERANCE;
}
bool test_aquire_multiple_permits() {
    // Similar to the basic test, but with various sized permits
    using namespace std::chrono;

    RateLimiterInterface* limiter = new RateLimiter();
    limiter->set_rate(0.5);

    long long start = GET_TIME;

    // This aquire should happen instantly, but cause the next aquire
    // to occur ~4 seconds later
    limiter->aquire(2);

    long long first = GET_TIME;

    limiter->aquire(1);

    long long end = GET_TIME;
    long long error = ((end - start) - 4000) / 4000;

    return (first - end < 100) && std::abs(error) < TOLERANCE;
}
bool test_try_aquire() {
    using namespace std::chrono;

    RateLimiterInterface* limiter = new RateLimiter();
    limiter->set_rate(0.5);

    // Make sure it checks inputs
    bool exception_thrown = false;
    try {
        limiter->try_aquire(-1);
    } catch (std::runtime_error ex) {
        exception_thrown = true;
    }

    if (exception_thrown) {
        return false;
    }

    long long start = GET_TIME;

    // This should pass, since the current request only
    // affects the next, and since this is the first, any size can go through
    assert(limiter->try_aquire(1));

    // Test the timeout. Since we have a 2 second window, this should fail
    assert(!limiter->try_aquire(1));

    // Do the same as before, but with a specified window
    assert(!limiter->try_aquire(1, 500));

    // The previous two requests should NOT affect future requests, as they
    // should have failed. Thus, we should have 2 seconds left, minus whatever
    // computation time was used in the first two. This should pass. If, for some
    // reason, the other 2 DID affect the future, then a six second window would be needed,
    // and this should fail.
    assert(limiter->try_aquire(1, 2000));

    long long end = GET_TIME;
    double error = ((end - start) - 2000) / 2000;

    return std::abs(error) < TOLERANCE;
}
bool test_rate() {
    using namespace std::chrono;

    RateLimiterInterface* limiter = new RateLimiter();

    // Initial test. Set rate to 1.0, measure
    limiter->set_rate(1);
    assert(limiter->get_rate() == 1);

    long long start = GET_TIME;

    limiter->aquire();
    limiter->aquire();

    long long end = GET_TIME;
    double first_error = ((end - start) - 1000) / 1000;

    limiter->set_rate(0.5);
    assert(limiter->get_rate() == 0.5);

    // This is where it gets interesting! The previous aquire()
    // set the wait time for the next aquire() to be 1 second, but we
    // changed the rate right after! However, the implementation doesn't
    // keep track of the last request, just when the new one can take place. Thus,
    // the new rate will be applied on the next aquire.

    limiter->aquire();

    // After this aquire, we will have waited the 1 second. We expect the next
    // one to happen in 2 seconds, instead of 1
    start = GET_TIME;
    limiter->aquire();
    end = GET_TIME;

    long long second_error = ((end - start) - 2000) / 2000;

    return std::abs(first_error) < TOLERANCE && std::abs(second_error) < TOLERANCE;
}

bool test_concurrent() {
    // Pretty basic concurrent test to make sure it's threadsafe.
    // It's pretty hard to test fully, but the general idea of this test
    // is to have a relatively high rate, and put a LOT of concurrent requests through.

    // In order for this to be thread safe, the following conditions must be satisfied:
    // 1) No Deadlocks
    // 2) No Starvation
    // 3) No Unexpected behaviour
    // If this test runs to completion, it's a strong indicator that it's deadlock free,
    // but not necessarily garuenteed (although the algorithm doesn't show any reason why
    // a deadlock would occur. Worst case is an erroneous time calculation that leads to a
    // REALLY long wait.
    using namespace std::chrono;

    RateLimiterInterface* limiter = new RateLimiter();
    limiter->set_rate(100.0);

    // We use futures, so we can make sure every task finishes.
    // Note: We cannot simply do a bunch of std::async without storing
    //       future, since it will trigger the future's destructor, causing it
    //       to be synchronous.
    std::future<bool> results[1000];

    long start = GET_TIME;
    for (int i = 0; i < 1000; i++)
        results[i] = std::async(std::launch::async, [limiter]() { limiter->aquire(); return true; });

    for (int i = 0; i < 1000; i++)
        assert(results[i].get());
    long end = GET_TIME;

    double error = ((end - start) - 10000.0) / 10000.0;

    return std::abs(error) < 10 * TOLERANCE;
}

struct Test {
    bool (*test)();
    std::string name;
};

Test tests[] = {
    { test_aquire, "Aquire" },
    { test_aquire_multiple_permits, "Aquire Multiple" },
    { test_rate, "Rate" },
    { test_try_aquire, "Try Aquire" },
    { test_concurrent, "Concurrency" }
};

int main() {
    using namespace std;

    int failed = 0;
    int num = sizeof(tests) / sizeof(Test);
    for (int i = 0; i < num; i++) {
        cout << "Running Test: " << tests[i].name << "...";
        cout.flush();

        if (tests[i].test())
            cout << "Passed.";
        else {
            cout << "Failed.";
            failed++;
        }

        cout << endl;
    }

    return failed;
}

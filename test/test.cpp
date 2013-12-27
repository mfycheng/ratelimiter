#include <cassert>
#include <chrono>
#include <iostream>
#include <cmath>
#include "rate_limiter.hpp"

#define GET_TIME duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

bool test_aquire() {
    // Basic test: Run a bunch of aquires, and make sure
    // it happens in the expected time (with some tolerance)
    using namespace std::chrono;
    
    RateLimiterInterface* limiter = new RateLimiter();
    limiter->set_rate(10);
    long start = GET_TIME
    
    for (int i = 0; i < 20; i++) {
        limiter->aquire();
    }
    
    long end = GET_TIME
    
    double error = ((end - start) - 2000) /  2000;
    
    return std::abs(error) < 0.01;
}
bool test_aquire_multiple_permits() {
    // Similar to the basic test, but with various sized permits
    using namespace std::chrono;
    
    RateLimiterInterface* limiter = new RateLimiter();
    limiter->set_rate(0.5);

    long start = GET_TIME
    
    // This aquire should happen instantly, but cause the next aquire
    // to occur ~4 seconds later
    limiter->aquire(2);
    
    long first = GET_TIME
    
    limiter->aquire(1);

    long end = GET_TIME
    long error = ((end - start) - 4000) / 4000;
    
    return (first - end < 100) && std::abs(error) < 0.01;
}
bool test_try_aquire() {
    using namespace std::chrono;
    
    RateLimiterInterface* limiter = new RateLimiter();
    limiter->set_rate(0.5);
    
    long start = GET_TIME
    
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
    
    long end = GET_TIME
    double error = ((end - start) - 2000) / 2000;
    
    return std::abs(error) < 0.01;
}

int main() {
    using namespace std;
    
    cout << "Running Aquire Test...";
    if (test_aquire())
        cout << "Passed." << endl;
    else
        cout << "Failed." << endl;
    
    cout << "Running Multiple Aquire Test...";
    if (test_aquire_multiple_permits())
        cout << "Passed." << endl;
    else
        cout << "Failed." << endl;
    
    cout << "Running try_aquire test...";
    if (test_try_aquire())
        cout << "Passed." << endl;
    else
        cout << "Failed." << endl;
}
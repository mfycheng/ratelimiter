#include <iostream>
#include <chrono>
#include "rate_limiter.hpp"

int main() {
    using namespace std::chrono;
    RateLimiterInterface* limiter = new RateLimiter();
    
    limiter->set_rate(1.0);
    
	long start = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    
    std::cout << "Starting..." << std::endl;
    for (int i = 0; i < 10; i++) {
        limiter->try_aquire(1, 1000);
        std::cout << "Aquired..." << std::endl;
    }
    
    long end = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    std::cout << "Finished in " << end - start << " milliseconds" << std::endl;
}
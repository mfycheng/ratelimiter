#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <thread>

#include "rate_limiter.hpp"

void RateLimiter::aquire() {
	aquire(1);
}
void RateLimiter::aquire(int permits) {
	if (permits <= 0)
		std::runtime_error("RateLimiter: Must request positive amount of permits");

	auto wait_time = claim_next(permits);
	std::this_thread::sleep_for(wait_time);
}

bool RateLimiter::try_aquire(int permits) {
    return try_aquire(permits, 0);
}
bool RateLimiter::try_aquire(int permits, int timeout) {
    return false;
}

void RateLimiter::sync(long now) {
    // If we're passed the next_free, then recalculate
    // stored permits, and update next_free_
    if (now > next_free_) {
        stored_permits_ = std::min(max_permits_, stored_permits_ + (now - next_free_) / interval_);
        next_free_ = now;
    }
}
std::chrono::microseconds RateLimiter::claim_next(double permits) {
	using namespace std::chrono;

	std::lock_guard<std::mutex> lock(mut_);

	long now = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
    
    // Make sure we're synced
    sync(now);
    
    // Since we synced before hand, this will always be >= 0.
	long wait = next_free_ - now;

    // Determine how many stored and freh permits to consume
	double stored = std::min(permits, stored_permits_);
	double fresh = permits - stored;

	// In the general RateLimiter, stored permits have no wait time,
	// and thus we only have to wait for however many fresh permits we consume
	long next_free = (long)(fresh * interval_);

	next_free_ += next_free;
	stored_permits_ -= stored;

	return microseconds(wait);
}

double RateLimiter::get_rate() const {
	return 1000000.0 / interval_;
}
void RateLimiter::set_rate(double rate) {
	if (rate <= 0.0)
		throw std::runtime_error("RateLimiter: Rate must be greater than 0");

	std::lock_guard<std::mutex> lock(mut_);
	interval_ = 1000000.0 / rate;
}
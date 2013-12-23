#ifndef _rate_limiter_h_
#define _rate_limiter_h_

class RateLimiter {
public:
	void aquire();
	void aquire(int permits);

	void try_aquire(int timeout);
	void try_aquire(int permits, int timeouts);
};

#endif
# Description [![Build Status](https://travis-ci.org/mfycheng/ratelimiter.png?branch=master)](https://travis-ci.org/mfycheng/ratelimiter)

ratelimiter is a simple c++ implementation of a thread safe, basic rate limiter.

# Usage

### Creating a Rate Limiter

There are multiple types of rate limiters, each with their own timing behaviour. However, they all implement
the same interface. To create a rate limiter, simply:

```
RateLimiterInterface* limiter = new RateLimiter();
limiter->set_rate(1.0); // 1 Permit per second
```

You can of course choose another rate limiter (warm up, for example). Alternatively, if the specific rate limiter's consturctor supports it:

```
RateLimiterInterface* limiter = new RateLimiter(1.0); // Creates a ratelimiter with 1 permit per second
```

### Using The Rate Limiter

There are two ways to aquire permits:

  * Blocking
  * Blocking with Timeout

For the basic blocking:

```
double wait_time = limiter->aquire(); // aquires 1 permit. Will block the thread until it is allowed to proceed
double wait_time = limiter->aquire(20); // aquires 20 permits. Will block the thread until it's allowed to proceed
```

Where wait_time is the amount of time that the thread was blocked for in milliseconds.

For the timeout blocking:

```
bool aquired = limiter->try_aquire(3000); // Same as try_aquire(1, 3000)
bool aquired = limiter->try_aquire(2, 3000);
```

On both cases, if try_aquire can aquire it's permits within the specified time (from now), it will block as long as necessary by the rate limiter, and then return true when it has aquired the permits. If it cannot aquire those permits within the specified time, then it will return IMMEDIATELY, with a value of false.

# General Notes

These apply to all rate limiters in this library.

 * The first aquire happens instantly
 * Permits don't need to be returned
 * Changing the rate of a rate limiter will be applied on the NEXT aquire. For more information, see the test_rate() test.
 * Aquiring permits affect the next aquire, and not the current. That is, the next aquire will happen at the same time regardless of how many permits it requires. The amount of permits it aquired affects how long until the next aquire will be allowed.

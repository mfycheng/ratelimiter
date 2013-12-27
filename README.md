# Description

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
bool aquired = limiter->try_aquire(); // attempts to grab 1 permit. If it cannot aquire immediately, returns false, with no block
bool aquired = limiter->try_aquire(2); // same as try_aquire(), but with 2 permits
bool aquired = limiter->try_aquire(2, 3000); // attemps to grab 2 permits, and WILL ONLY BLOCK if it can aquire those permits WITHIN 3 seconds.
```

Essentially, if try_aquire can get it's permits within the specified time, then it will block, and then return true. Otherwise, it will return immediately and return false.

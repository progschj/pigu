#include <time.h>
#include <inttypes.h>

uint64_t PIGU_raw_time()
{
   struct timespec t;
   clock_gettime(CLOCK_MONOTONIC, &t);
   return (uint64_t)t.tv_sec * (uint64_t)1000000000 + (uint64_t)t.tv_nsec;
}

double piguGetTime()
{
   static uint64_t base = 0;
   if(base == 0)
      base = PIGU_raw_time();
   return 1.e-9*(PIGU_raw_time() - base);
}

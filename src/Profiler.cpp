#include "Profiler.h"

#include <sys/time.h>

double Profiler::now() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return tv.tv_sec + 1e-6 * tv.tv_usec;
}

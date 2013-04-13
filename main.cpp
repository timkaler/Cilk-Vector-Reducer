#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <vector>

#include "vector_reducer.h"

// Simple timer for benchmarks
double get_time()
{
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    return t.tv_sec + t.tv_usec*1e-6;
}

int main(int argc, char **argv)
{
  Vector_reducer<int> v;

  double add_start = get_time();
  cilk_for(int i = 0; i < 100000000; i++) {
    v.insert(i);
  }
  double add_end = get_time();

  double reducing_start = get_time();
  std::vector<int>& combined = v.get_reference();
  double reducing_end = get_time();

  printf("\n");
  printf("Time adding elements: %f \n", add_end - add_start);
  printf("Time merging elements: %f \n", reducing_end - reducing_start);
  printf("Total time: %f \n", reducing_end - add_start);
  printf("\n");

  // Verify that the contents appear in order.
  for (int i = 0; i < combined.size(); i++) {
    if (combined[i] != i) {
      printf("Error! Contents not in serial order.\n");
      return 0;
    }
  }

  return 0;
}


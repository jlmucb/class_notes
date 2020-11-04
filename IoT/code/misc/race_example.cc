#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <mutex>

// Example of race condition

#define NUMTHREADS 4
#define NCYCLES 100

int g_count = 0;

#define STOP_RACE

#ifdef STOP_RACE
std::mutex my_mutex;
#endif


void* worker(void* arg) {
  int n;

  srand(5);

  for (int k = 0; k < NCYCLES; k++) {
#ifdef STOP_RACE
    my_mutex.lock();
#endif
    n = *((int*)arg);

    int m = rand();
    if ((m%4) == 1)
      sleep(1);
    n++;
    *((int*)arg) = n;
#ifdef STOP_RACE
    my_mutex.unlock();
#endif
  }
  return nullptr;
}

int main(int an, char** av) {
  pthread_t threads[NUMTHREADS];

  for (int i = 0; i < NUMTHREADS; i++) {
    pthread_create(&threads[i], NULL, worker, (void*) &g_count);
  }

  for (int i = 0; i < NUMTHREADS; i++) {
    int r = pthread_join(threads[i], NULL);
    printf("Thread %d terminated\n", i);
  }
  printf("raced count: %d, should be: %d\n", g_count, NUMTHREADS * NCYCLES);
  return 0;
}


// restaurant.c
/*
//*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define CAPACITY 10
#define EMPTY 0

extern int rand_r(unsigned int*);

int used_seats=0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


/*Method for visit.
 * A customer comes in, waits some (random) time and leaves
 */
void* visitRestaurant (void *arg){
  int customer_id = (int) arg;
  unsigned int seed = (unsigned int) customer_id;
  ++customer_id; // starting the numbers with 1 ;-)
  while(pthread_mutex_trylock(&lock));
  used_seats++;
  pthread_mutex_unlock(&lock);
  printf("Customer %d (Thread %lu) -- Entering Restaurant\n", customer_id,(unsigned long) pthread_self());
  sleep(rand_r(&seed)%10);
  printf("Customer %d (Thread %lu) -- Leaving Resaturant\n", customer_id,(unsigned long) pthread_self());
  while(pthread_mutex_trylock(&lock));
  used_seats--;
  pthread_mutex_unlock(&lock);
  return NULL;   
}


int main(){
  int c;
  pthread_t tids[CAPACITY];    //Array of Threads
  
  //Create a number of threads
  for (c=0; c<CAPACITY; c++){
    pthread_create(&tids[c],NULL,visitRestaurant,(void*)c);
    sleep(rand()%5);
  }

  //Wait for every thread
  for (c=0;c<CAPACITY;c++){
    pthread_join(tids[c],NULL);
  }
  //used seats shoult be 0 after execution of all threads
  printf("Total: %d\n",used_seats);
  return 0;   
}

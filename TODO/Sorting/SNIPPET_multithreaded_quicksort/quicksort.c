// quicksort.c
/*
 * multiple-thread quick-sort (c) SUN 1995.  
 * 
 * See man page for qsort(3c) for info.
 * Works fine on uniprocessor machines as well.
 *
 * Written by:  Richard Pettit (Richard.Pettit@West.Sun.COM)
 * 
 * ATTENTION:
 * don't print out MACROS, they will be called twice due to text replacement!!
 */

// FIXME: comparator doesn't work
// FIXME: output not sorted!


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

extern int pthread_getconcurrency();
extern int pthread_setconcurrency(int);


// don't create more threads for less than this
#define SLICE_THRESH   4096

// how many threads per lwp 
#define THR_PER_LWP       4

// data structure for the sort arguments
typedef struct {
  // base pointer - points to the current element
  void    *sortargs_base;

  // number of elements
  int      sortargs_number_of_elements;

  // width of the current element, offset - size in bytes
  size_t   sortargs_width;

  // function pointer to the comparison function
  int    (*sortargs_compar_func)(const void *, const void *);
} sort_args_t;


// for all instances of quicksort
static int threads_avail;


// MACRO to cast the void to a one byte quanitity and compute the offset
#define SUB(base_pointer, number) \
((void *) (((unsigned char *) (base_pointer)) + ((number) * width)))


// MACRO to swap
#define SWAP(base_pointer, idx, jdx, width) \
{ \
  int number_of_elements; \
 \
/* vars for the offset of the different types */ \
  unsigned char      uc; \
  unsigned short     us; \
  unsigned long      ul; \
  unsigned long long ull; \
 \
/* treat pivot */ \
  if(SUB(base_pointer, idx) == pivot){ \
    pivot = SUB(base_pointer, jdx); \
  }else if(SUB(base_pointer, jdx) == pivot){ \
    pivot = SUB(base_pointer, idx); \
  } \
 \
/* one of the more convoluted swaps I've done */ \
  switch(width){ \
  case 1: \
    uc = *((unsigned char *) SUB(base_pointer, idx)); \
    *((unsigned char *) SUB(base_pointer, idx)) = *((unsigned char *) SUB(base_pointer, jdx)); \
    *((unsigned char *) SUB(base_pointer, jdx)) = uc; \
    break; \
 \
  case 2: \
    us = *((unsigned short *) SUB(base_pointer, idx)); \
    *((unsigned short *) SUB(base_pointer, idx)) \
= *((unsigned short *) SUB(base_pointer, jdx)); \
    *((unsigned short *) SUB(base_pointer, jdx)) = us; \
    break; \
 \
  case 4: \
    ul = *((unsigned long *) SUB(base_pointer, idx)); \
    *((unsigned long *) SUB(base_pointer, idx)) = *((unsigned long *) SUB(base_pointer, jdx)); \
    *((unsigned long *) SUB(base_pointer, jdx)) = ul; \
    break; \
 \
  case 8: \
    ull = *((unsigned long long *) SUB(base_pointer, idx)); \
    *((unsigned long long *) SUB(base_pointer, idx)) = *((unsigned long long *) SUB(base_pointer,jdx)); \
    *((unsigned long long *) SUB(base_pointer, jdx)) = ull; \
    break; \
 \
  default: \
    for(number_of_elements = 0; number_of_elements < width; ++number_of_elements){ \
    /* set using offset */ \
      uc = ((unsigned char*) SUB(base_pointer, idx))[number_of_elements]; \
    /* swap */ \
    ((unsigned char*) SUB(base_pointer, idx))[number_of_elements] = ((unsigned char*) SUB(base_pointer, jdx))[number_of_elements]; \
    /* reset by using offset */ \
    ((unsigned char*) SUB(base_pointer, jdx))[number_of_elements] = uc; \
    } \
    break; \
  } \
} 


/*
  called by quicksort() - does the actual sorting

  arg - to pass the structure of the sorting arguments
//*/
static void* _quicksort(void *arg)
{
  // set the sorting arguments locally
  sort_args_t *sargs = (sort_args_t *) arg;

  // base pointer - current pointer to the element, set to base
  register void *base_pointer = sargs->sortargs_base;

  // number of elements (size)
  int number_of_elements = sargs->sortargs_number_of_elements;
  
  // the offset size of the elements (each?)
  int width = sargs->sortargs_width;

  // set the comparison function (pointer) to the function set in the arguments
  int (*compar_func)(const void *, const void *) = sargs->sortargs_compar_func;

  // counding indexes
  register int idx;
  register int jdx;

  // the result of the comparison
  int result;
  int thread_count = 0;

  // tmp - temporary storage for an element at swapping
  void *tmp;

  // tmp_base - temporary base pointer, pointer to an element
  void *tmp_base[3];

  void *pivot = 0;
  sort_args_t sort_args[2];
  pthread_t tid;

  // find the pivot point
  switch(number_of_elements){
  case 0:
  case 1:
    return 0;

  case 2:
    if((*compar_func)(SUB(base_pointer, 0), SUB(base_pointer, 1)) > 0){
      SWAP(base_pointer, 0, 1, width);
    }
    return 0;

  case 3:
    // three sort 
    if((*compar_func)(SUB(base_pointer, 0), SUB(base_pointer, 1)) > 0){
      SWAP(base_pointer, 0, 1, width);
    }

    // the first two are now ordered, now order the second two
    if((*compar_func)(SUB(base_pointer, 2), SUB(base_pointer, 1)) < 0){
      SWAP(base_pointer, 2, 1, width);
    }

    // should the second be moved to the first?
    if((*compar_func)(SUB(base_pointer, 1), SUB(base_pointer, 0)) < 0){
      SWAP(base_pointer, 1, 0, width);
    }
    return 0;

  default:
    if(number_of_elements > 3){
      tmp_base[0] = SUB(base_pointer, 0);
      tmp_base[1] = SUB(base_pointer, number_of_elements / 2);
      tmp_base[2] = SUB(base_pointer, number_of_elements - 1);

      // three sort
      if((*compar_func)(tmp_base[0], tmp_base[1]) > 0){
        tmp = tmp_base[0];
        tmp_base[0] = tmp_base[1];
        tmp_base[1] = tmp;
      }

      // the first two are now ordered, now order the second two
      if((*compar_func)(tmp_base[2], tmp_base[1]) < 0){
        tmp = tmp_base[1];
        tmp_base[1] = tmp_base[2];
        tmp_base[2] = tmp;
      }

      // should the second be moved to the first?
      if((*compar_func)(tmp_base[1], tmp_base[0]) < 0){
        tmp = tmp_base[0];
        tmp_base[0] = tmp_base[1];
        tmp_base[1] = tmp;
      }
      if((*compar_func)(tmp_base[0], tmp_base[2]) != 0){
        if((*compar_func)(tmp_base[0], tmp_base[1]) < 0){
          pivot = tmp_base[1];
	}else{
          pivot = tmp_base[2];
	}
      }
    }
    break;
  }
  if(pivot == 0){
    for(idx = 1; idx < number_of_elements; ++idx){
      if(0 != (result = ((*compar_func) (SUB(base_pointer, 0), SUB(base_pointer, idx))))){
        pivot = (result > 0) ? SUB(base_pointer, 0) : SUB(base_pointer, idx);
        break;
      }
    }
  }
  if (pivot == 0)
    return 0;
  
  /*
    sort
    idx - starting from 0
    jdx - starting from the last elements index
    base_pointer - pointer to the current element
  //*/
  idx = 0;
  jdx = number_of_elements - 1;
  while(idx <= jdx){
    while((*compar_func)(SUB(base_pointer, idx), pivot) < 0)
      ++idx;
    while((*compar_func)(SUB(base_pointer, jdx), pivot) >= 0)
      --jdx;
    if(idx < jdx){
      SWAP(base_pointer, idx, jdx, width);
      ++idx;
      --jdx;
    }
  }
  
  // sort the sides judiciously
  switch(idx){
  case 0:
  case 1:
    break;

  case 2:
    if((*compar_func)(SUB(base_pointer, 0), SUB(base_pointer, 1)) > 0) {
      SWAP(base_pointer, 0, 1, width);
    }
    break;

  case 3:
    // three sort 
    if((*compar_func)(SUB(base_pointer, 0), SUB(base_pointer, 1)) > 0) {
      SWAP(base_pointer, 0, 1, width);
    }

    // the first two are now ordered, now order the second two
    if ((*compar_func)(SUB(base_pointer, 2), SUB(base_pointer, 1)) < 0) {
      SWAP(base_pointer, 2, 1, width);
    }

    // should the second be moved to the first?
    if ((*compar_func)(SUB(base_pointer, 1), SUB(base_pointer, 0)) < 0) {
      SWAP(base_pointer, 1, 0, width);
    }
    break;

  default:
    // reinit the sorting arguments
    sort_args[0].sortargs_base                = base_pointer;
    sort_args[0].sortargs_number_of_elements  = idx;
    sort_args[0].sortargs_width               = width;
    sort_args[0].sortargs_compar_func         = compar_func;

    // call the _quicksort method recursively, creating threads if possible
    if((threads_avail > 0) && (idx > SLICE_THRESH)) {
      threads_avail--;
      pthread_create(&tid, NULL, _quicksort, &sort_args[0]);
      thread_count = 1;
    } else      
      _quicksort(&sort_args[0]);
    break;
  }

  // shrink
  jdx = number_of_elements - idx;
  switch(jdx){
  case 1:
    break;

  case 2:
    if((*compar_func)(SUB(base_pointer, idx), SUB(base_pointer, idx + 1)) > 0){
      SWAP(base_pointer, idx, idx + 1, width);
    }
    break;

  case 3:
    // three sort
    if((*compar_func)(SUB(base_pointer, idx), SUB(base_pointer, idx + 1)) > 0){ 
      SWAP(base_pointer, idx, idx + 1, width);
    }

    // the first two are now ordered, now order the second two
    if((*compar_func)(SUB(base_pointer, idx + 2), SUB(base_pointer, idx + 1)) < 0){
      SWAP(base_pointer, idx + 2, idx + 1, width);
    }

    // should the second be moved to the first?
    if((*compar_func)(SUB(base_pointer, idx + 1), SUB(base_pointer, idx)) < 0) {
      SWAP(base_pointer, idx + 1, idx, width);
    }
    break;

  default:
    // reinit the sorting arguments
    sort_args[1].sortargs_base               = SUB(base_pointer, idx);
    sort_args[1].sortargs_number_of_elements = jdx;
    sort_args[1].sortargs_width              = width;
    sort_args[1].sortargs_compar_func        = compar_func;

    // call _quicksort recursively, creating threads if possible
    if((thread_count == 0) && (threads_avail > 0) && (idx > SLICE_THRESH)) {
      threads_avail--;
      pthread_create(&tid, NULL, _quicksort, &sort_args[1]);
      thread_count = 1;
    }else{
      _quicksort(&sort_args[1]);
    }
    break;
  }
  if(thread_count){
    pthread_join(tid, NULL);
    threads_avail++;
  }
  return 0;
}


/*
  function to be called
  base_pointer          the base pointer to a element
  number_of_elements    the number of elements to sort
  width                 offset of an element
  compar_func           the comparison function
//*/
void quicksort(void* base_pointer
	       , size_t number_of_elements
	       , size_t width
	       , int (*compar_func)(const void*, const void *))
{
  static int number_of_processors = -1;

  /*
    get the number of processors and do some performance related settings
  //*/
  if(number_of_processors == -1){
    // get the number of processors (= cpu's)
    number_of_processors = sysconf( _SC_NPROCESSORS_ONLN);
    
    // lwp for each cpu
    if((number_of_processors > 1) && (pthread_getconcurrency() < number_of_processors))
      pthread_setconcurrency(number_of_processors);
    
    // thread count not to exceed THR_PER_LWP per lwp
    threads_avail = (number_of_processors == 1) ? 0 : (number_of_processors * THR_PER_LWP);
  }

  /*
    set up the sorting arguments
  //*/
  sort_args_t sort_args;
  sort_args.sortargs_base = base_pointer;
  sort_args.sortargs_number_of_elements = number_of_elements;
  sort_args.sortargs_width = width;
  sort_args.sortargs_compar_func = compar_func;

  // start sorting
  _quicksort(&sort_args);
}



/***************************************************************
 *
 * main and comparator...
 *
 ***************************************************************/

/*
  comparator - needs to destinguish between >0 or <0
//*/
int comp(const void* a, const void* b)
{
  int val_a = (int) a;
  int val_b = (int) b;

  if(val_a == val_b){
    return 0;
  }else if(val_a > val_b){
    return 1;
  }else{
    return -1;
  }
}


/*
  main
//*/
int main()
{
  int arr[10];

  int idx;
  for(idx = 0; idx < 10; ++idx)
    arr[idx] = rand();

  puts("unsorted:");
  for(idx = 0; idx < 10; ++idx)
    printf("[%d] - %d\n", idx, arr[idx]);

  int (*funcptr)(const void*, const void*) = comp;
  quicksort((void*) &arr[0], 10, sizeof(arr[0]), funcptr);

  puts("sorted:");
  for(idx = 0; idx < 10; ++idx)
    printf("[%d] - %d\n", idx, arr[idx]);

  puts("READY.");
  exit(EXIT_SUCCESS);
}

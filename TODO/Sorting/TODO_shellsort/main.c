// main.c
/*
//*/

#include "main.h"
#include "shellsort.h"


/*
  print the list
//*/
void printlist(void *arg)
{
  element_t *tmp = ( element_t * )arg;
  printf("%02d ", tmp->data);

  while (NULL != (tmp = tmp->next)) {
    printf("%02d ", tmp->data);
  }
  printf("\n");
}


/*
  allocate elements for the list
//*/
void alloclist(void *start, void *end)
{
  element_t **first = ( element_t ** )start;
  element_t **last = ( element_t ** )end;
  element_t *tmp = NULL;

  unsigned int idx = 0;
  for (idx = 0; idx < MAX_LIST_SIZE; ++idx) {
    if (NULL == (tmp = malloc(sizeof(*tmp)))) {
      perror("malloc failed!");
      freelist(( void * )first);
      exit(EXIT_FAILURE);
    }
    ++global_list_size;

    // limit to values up to 100
    tmp->data = random() % 100; // XXX
    if (0 == idx) {
      tmp->prev = NULL;
      tmp->next = NULL;
      *first = tmp;
    } else {
      tmp->prev = *last;
      tmp->next = NULL;
      (*last)->next = tmp;
    }
    *last = tmp; // XXX
    tmp = NULL;
  }
  tmp = NULL;
}


/*
  free the elements for the list
//*/
void freelist(void *init)
{
  if (global_list_size != MAX_LIST_SIZE) {
    fprintf(stderr, "ERROR: global_list_size (%d) != MAX_LIST_SIZE (%d)\n",
            global_list_size, MAX_LIST_SIZE);
  }

  element_t **first = ( element_t ** )init;
  if (NULL == *first)
    return;

  element_t *tmp = NULL;

  register unsigned int idx = 0;
  for (idx = 0; idx < global_list_size; ++idx) {
    tmp = *first;
    printf("%02d ", tmp->data);
    *first = tmp->next;
    free(tmp);
  }
  global_list_size = 0;
  printf("\n");
}


/*
  application's main
//*/
int main()
{
  element_t *first = NULL;
  element_t *last = first;
  global_list_size = 0;

  // alloc
  puts("alloc");
  alloclist(( void * )&first, ( void * )&last);
  printf("\n");

  // output
  puts("elements - before");
  printlist(first);
  printf("\n");

  puts("sorting");
  sort(( void * )&first);
  printf("\n");

  puts("elements - after");
  printlist(first);
  printf("\n");

  // free
  puts("free");
  freelist(( void * )&first);
  printf("\n");

  // exit
  puts("READY.");
  exit(EXIT_SUCCESS);
}

// main.c
/*
//*/

#include "main.h"
#include "bubblesort.h"

/*
  print the list
//*/
void printlist(void *arg)
{
  element *tmp = ( element * )arg;
  printf("%02d ", tmp->value);

  while (NULL != (tmp = tmp->next)) {
    printf("%02d ", tmp->value);
  }
  printf("\n");
}


/*
  allocate elements for the list
//*/
void alloclist(void *start, void *end)
{
  element **first = ( element ** )start;
  element **last = ( element ** )last;

  element *tmp = NULL;

  unsigned int idx = 0;
  for (idx = 0; idx < LIST_SIZE; ++idx) {
    if (NULL == (tmp = malloc(sizeof(*tmp)))) {
      perror("malloc failed!");
      exit(EXIT_FAILURE);
    }
    tmp->value = random();
    // limit to values up to 100
    tmp->value = (tmp->value % 100);
    if (0 == idx) {
      tmp->prev = NULL;
      tmp->next = NULL;
      *first = tmp;
    } else {
      tmp->prev = *last;
      tmp->next = NULL;
      (*last)->next = tmp;
    }
    *last = tmp;
    tmp = NULL;
  }
  tmp = NULL;
}


/*
  free the elements for the list
//*/
void freelist(void *init)
{
  element **first = ( element ** )init;
  if (NULL == *first)
    return;

  element *tmp = NULL;

  register unsigned int idx = 0;
  for (idx = 0; idx < LIST_SIZE; ++idx) {
    tmp = *first;
    printf("%02d ", tmp->value);
    *first = tmp->next;
    free(tmp);
  }
  printf("\n");
}


/*
  application's main
//*/
int main()
{
  element *first = NULL;
  element *last = first;

  // alloc
  puts("alloc");
  alloclist(&first, &last);
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
  freelist(&first);
  printf("\n");

  // exit
  puts("READY.");
  exit(EXIT_SUCCESS);
}

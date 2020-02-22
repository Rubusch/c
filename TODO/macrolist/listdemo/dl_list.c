// dl_list.c
/*
  double linked list

  The main difference between a single and double linked list consists in
  more work on the linking, of course. But it is much easier to insert or
  remove elements in the middle of the list now, because a double linked
  list allows iterating upwards AND DOWNWARDS (which is a big advantage)!!!

  E.g. when removing an element, it is not necessary additionaly to run
  through all elements to figure out the element before in order to reset
  the linkage properly. Now we can simply use the prev-link of the element
  directly.
//*/

#include "dl_list.h"

// internal usage
static element_t *find(const char *, const unsigned int);
static int append(element_t *);
static int discard(element_t **);
static element_t *getnew();
static int init(element_t *, const char *, const unsigned int);

// first and last pointer
static element_t *first = NULL;
static element_t *last = NULL;


/*
  removes the whole list
//*/
int removeall()
{
  while (first) {
    if (0 != removeelement(first->data, 1 + strlen(first->data))) {
      return -1;
    }
  }

  return 0;
}


/*
  appends an element to the end of the list
    data       data of the element to append
    data_size  size of data
//*/
int appendelement(const char *data, const unsigned int data_size)
{
  // declaration
  element_t *tmp = NULL;

  // allocate space
  if (NULL == (tmp = getnew())) {
    return -1;
  }

  // init
  if (0 != init(tmp, data, data_size)) {
    discard(&tmp);
    return -1;
  }

  // append
  if (0 != append(tmp)) {
    discard(&tmp);
    return -1;
  }

  return 0;
}


/*
  inserts an element after a given element (prev_data)

    prev_data       content of the previous element
    data            content of the new element
    data_size       size of data
//*/
int insertelement(const char *prev_data, const unsigned int prev_data_size,
                  const char *data, const unsigned int data_size)
{
  // checks
  if (NULL == data)
    return -1;

  // if prev_data is null - prepend
  if (NULL == prev_data) {
    element_t *tmp = NULL;
    if (NULL == (tmp = getnew())) {
      return -1;
    }
    if (0 != init(tmp, data, data_size)) {
      return -1;
    }

    // relink - last is already set correctly
    tmp->next = first;
    first = tmp;

    // if tmp is the only element added, set last, too
    if (NULL == last) {
      last = first;
    }

    return 0;
  }

  // prev elements
  element_t *prev = find(prev_data, prev_data_size);
  if (NULL == prev) {
    fprintf(stderr, "element \"%s\" not found in list\n", prev_data);
    return -1;
  }

  // new element
  element_t *tmp = find(data, data_size);
  if (NULL == tmp) {
    tmp = getnew();
    init(tmp, data, data_size);
  } else {
    fprintf(stderr, "the element \"%s\" is alredy contained in the list\n",
            data);
    return -1;
  }

  // element is going to be appended
  element_t *next = prev->next;
  if (NULL == next) {
    return appendelement(data, data_size);
  }

  // or element is going to be inserted - relink elements
  tmp->next = next;
  next->prev = tmp;
  prev->next = tmp;
  tmp->prev = prev;

  return 0;
}


/*
  removes an element in the list

    data       data of the element to find
    data_size  size of data
//*/
// FIXME: cut's of elements before!!!
int removeelement(const char *data, const unsigned int data_size)
{
  // checks
  if (NULL == data)
    return -1;

  // find and delete
  element_t *tmp = find(data, data_size);
  if (NULL == tmp) {
    fprintf(stderr, "element \"%s\" not found in list\n", data);
    return -1;
  }

  // neighbours
  element_t *prev = tmp->prev;
  element_t *next = tmp->next;

  // relink prev
  if (NULL != prev) {
    prev->next = next;
  } else {
    first = next;
  }

  // relink next
  if (NULL != next) {
    next->prev = prev;
  } else {
    last = prev;
  }

  // remove tmp
  if (0 != discard(&tmp)) {
    perror("discard failed");
    return -1;
  }
  tmp = NULL;

  return 0;
}


/* ******************************************************************************

  the following section are functions internally used in the list

//
******************************************************************************/


/*
  prints the content of the whole list
//*/
int printlist()
{
  printf("content: ");
  element_t *tmp = first;
  if (NULL == tmp) {
    printf(" -\n");
    return 0;
  }
  while (tmp) {
    printf("%s  ", tmp->data);
    tmp = tmp->next;
  }
  printf("\n");

  return 0;
}


/*
  find an element by its data

    data       data of the element to find
    data_size  size of data

    returns a pointer to the element containing data or NULL
//*/
static element_t *find(const char *data, const unsigned int data_size)
{
  if (NULL == data)
    return NULL;

  element_t *tmp = first;
  while (tmp) {
    if (0 == strncmp(tmp->data, data, data_size)) {
      return tmp;
    }
    tmp = tmp->next;
  }

  return NULL;
}


/*
  appends an element to the list
    tmp            element to append
//*/
static int append(element_t *tmp)
{
  // checks
  if (NULL == tmp)
    return -1;

  if (NULL == first) {
    // tmp is first element
    first = tmp;
    last = first;
  } else {
    // tmp is a trivial element to append
    last->next = tmp;
    tmp->prev = last;
    last = tmp;
  }

  return 0;
}


/*
  removes and frees the space of an element
    tmp       the elment to remove
//*/
static int discard(element_t **tmp)
{
  // checks
  if (NULL == *tmp)
    return -1;

  // free the element
  if (NULL != (*tmp)->data)
    free((*tmp)->data);
  free(*tmp);

  // reset to NULL
  *tmp = NULL;

  return 0;
}


/*
  allocates space for a new element

  returns a pointer to the new element
//*/
static element_t *getnew()
{
  element_t *tmp = NULL;

  // allocate
  if (NULL == (tmp = malloc(sizeof(*tmp)))) {
    perror("allocation failed");
    exit(EXIT_FAILURE);
  }

  // return it
  return tmp;
}


/*
  init the value of an element
    tmp          element
    data         data value
    data_size    size of data
//*/
static int init(element_t *tmp, const char *data, const unsigned int data_size)
{
  // checks
  if (NULL == tmp)
    return -1;
  if (DATASIZ < data_size)
    return -1;

  // init
  tmp->data = NULL;
  tmp->next = NULL;
  tmp->prev = NULL;

  // alloc
  if (NULL == (tmp->data = calloc(data_size, sizeof(tmp->data)))) {
    perror("allocation failed");
    return -1;
  }

  // init
  strncpy(tmp->data, data, data_size);

  return 0;
}

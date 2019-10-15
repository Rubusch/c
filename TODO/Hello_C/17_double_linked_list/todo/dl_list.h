// dl_list.h
/*
  A double linked list works like that:

  <-prev\
  ->element<-prev\
      \next->element<-prev\
               \next->element
                        \next->NULL
//*/

// TODO

#define DATASIZ 64

typedef struct element {
  struct element *next;
  struct element *prev;
  char *data;
} element_t;

// external usage interface level

// TODO

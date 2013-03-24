// sl_list.c
/*
  single linked list

  demonstrates: typedef / struc and the use of several files and make

  return value 
  As we see here, the return value gives information about the exit 
  status of the called function. This is one of the main purposes of 
  a return value. If 0 is returned everything went fine, in case of 
  an error -1 was returned.
  This example here demonstrates a quite exagerated usage of the return
  mechanism, normally only or at least the defined interface (e.g. here 
  the external calls, accessible from main.c) should provide some 
  mechanism like that. 
  Though it depends to the situation if it is important to check if 
  or not to check. As we can see here, sometimes passed pointers are 
  checked and sometimes it doesn't makes sense.

  Where to place checks?
  For each initialization or allocation one place should be declared 
  as responsable, this has to be checked. Checks at other places are 
  redundant checks, and in case they fail the "responsable code section"
  has to be corrected.
  
  fprintf()
  fprintf() uses the error stream, like perror() but fprintf() has the 
  advantage that parameters can be print out.
  
  outer and inner interface
  - some methods are meant to be for external usage -> .h file
  - others are meant to be used only internally -> static 
//*/

// TODO

// internal usage
static element_t* find(const char*, const unsigned int);
static int append(element_t*);
static int discard(element_t**);
static element_t* getnew();
static int init(element_t*, const char*, const unsigned int);

// first and last pointer
static element_t* first=NULL;
static element_t* last=NULL;


/*
  removes the whole list
//*/
int removeall()
{
  while(first){
    // TODO: use removelastelement()
  }

  return 0;
}


/*
  appends an element to the end of the list
  
    data       data of the element to append
    data_size  size of data
//*/
int appendelement(const char* data, const unsigned int data_size)
{
  // declaration
  element_t* tmp=NULL;

  // allocate space
  // TODO use getnew()

  // init
  // TODO use init()

  // append
  // TODO use append()

  return 0;
}


/*
  removes the last element
//*/
int removelastelement()
{
  // checks
  if(NULL == first) return -1;

  // get previous element
  element_t* prev = first;
  if(first != last){
    // TODO: go thru the elements using while()
  }else{
    first = NULL;
  }
  
  // remove
  // TODO: use discard()

  // relink
  // TODO

  return 0;
}


/*
  inserts an element after a given element (prev_data)
  
    prev_data       content of the previous element
    data            content of the new element
    data_size       size of data
//*/
int insertelement(const char* prev_data, const unsigned int prev_data_size, const char* data, const unsigned int data_size)
{
  // checks
  if(NULL == data) return -1;

  // special case: if prev_data is null - prepend the new element
  if(NULL == prev_data){
    element_t* tmp = NULL;
    if(NULL == (tmp = getnew())){
      return -1;
    }
    if(0 != init(tmp, data, data_size)){
      return -1;
    }

    // relink - last is already set correctly
    tmp->next = first;
    first = tmp;

    // if tmp is the only element added, set last, too
    if(NULL == last){ 
      last = first; 
    }

    return 0;
  }

  // normal case: prev elements
  // TODO get prev

  // new element
  element_t* tmp = find(data, data_size);
  // TODO: what if no tmp was found

  // next element
  element_t* next = prev->next;
  // TODO, the next

  // relink elements
  tmp->next = next;
  prev->next = tmp;

  return 0;
}


/* ******************************************************************************

  the following section are functions internally used in the list

// ******************************************************************************/


/*
  removes an element within the list
    
    data       data of the element to find
    data_size  size of data
//*/
int removeelement(const char* data, const unsigned int data_size)
{
  // checks
  if(NULL == data) return -1;

  // find and delete
  element_t* tmp = find(data, data_size);
  if(NULL == tmp){
    fprintf(stderr, "element \"%s\" not found in list\n", data);
    return -1;
  }

  // element was first
  if(tmp == first){

    // list only has one element
    if(first == last){
      // TODO use discard first.. reset elements
      return 0;
    }

    first = first->next;
    if(0 != discard(&tmp)){
      return -1;
    }
    tmp = NULL;
    return 0;
  }

  // get prev element
  element_t* prev = first;
  while(prev->next){
    if(prev->next == tmp){
      break;
    }
  }
  
  // next element
  element_t* next = prev->next;
  if(NULL == next){
    return removelastelement();
  }

  // unlink
  prev->next = next;

  // delete
  if(0 != discard(&tmp)){
    return -1;
  }

  return 0;
}


/*
  prints the content of the whole list
//*/
int printlist()
{
  // TODO implement printlist() that it works the way like in the output
  return 0;
}


/*
  find an element by its data

    data       data of the element to find
    data_size  size of data

    returns a pointer to the element containing data or NULL
//*/
static element_t* find(const char* data, const unsigned int data_size)
{
  // TODO - implement the find()
  return NULL;
}


/*
  appends an element to the list
    tmp            element to append
//*/
static int append(element_t* tmp)
{
  // checks
  if(NULL == tmp) return -1;

  if(NULL == first){
    // tmp is first element
    first = tmp;
    last = first;
  }else{ 
    // tmp is a trivial element to append
    last->next = tmp;
    last = tmp;
  }
  return 0;
}


/*
  removes and frees the space of an element

  tmp       the elment to remove
//*/
static int discard(element_t** tmp)
{
  // checks
  if(NULL == *tmp) return -1;
  
  // free the element
  if(NULL != (*tmp)->data) free((*tmp)->data);
  free(*tmp);
  
  // reset to NULL
  *tmp = NULL;

  return 0;
}


/*
  allocates space for a new element

  returns a pointer to the new element
//*/
static element_t* getnew()
{
  element_t* tmp = NULL;

  // allocate
  // TODO

  // return it
  return tmp;
}


/*
  init the value of an element
    tmp          element
    data         data value 
    data_size    size of data
//*/
static int init(element_t* tmp, const char* data, const unsigned int data_size)
{
  // TODO
  return 0;
}

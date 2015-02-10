// allocation.c
/*
  Allocation can be done with malloc() for single elements or calloc()
  for a list of elements. If a list of elements needs to be extended,
  this can be done by realloc().

  Though, realloc() is not so easy to use and can lead to hidden memory 
  leaks, since realloc() like all of the allocation functions can fail
  when the system is not able to allocate memory.


  pass of pointers in functions essentiells:

  we pass a pointer directly, when...
  - in a sub-function, we like to change the value of the variable to 
  which the pointer points to, we pass a pointer directly

  we pass a pointer to the pointer OR the address of a pointer, when...
  - in a subfunction, we want to change the address to where the pointer 
  points to, e.g. make it point to another variable in the subfunction
  - or we change the allocated memory where the pointer points to 
  (allocation, reallocation or free space)

  (see more -> pass-by-value, pass-by-reference)


  types:
  size_t    is an internal type for "sizes" and corresponds to some int, 
  alternatively int should work, as well (it is more conventional to use 
  size_t for sizes of arrays)


  the example trys to use subfunctions very often, this is not only a way
  to complicate your life - it's a method of abstraction. 
  pointers are a start to this kind of abstraction, functions and passing 
  pointers are other BASIC elements to this, continuing with void pointers
  What do we gain in working abstract?
  The answer is: portability and flexibility in code! You NEVER should 
  code towards the functionality, but towards interfaces. With interfaces 
  you will need a little bit more time to learn the techniques, but 
  afterwards, when changing some code, most of the changes will happen in 
  the executing implementation and not in the abstraction of an algorithm.

  You're frying potatoes, eggs and meat in the same pan and don't buy a 
  separate gadget for each one of them!
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
  reallocation in subfunction

  returns 0 in case of success and -1 in case of error
//*/
int get_more_space(char** str, unsigned int* str_size, const unsigned int how_much_more)
{
  char* tmp = NULL;

  // realloc() frees *str ???
  if(NULL == (tmp = realloc(*str, (*str_size + how_much_more)))){
    // if the allocation failed, free the already reallocated memory
    if(tmp!=NULL){
      free(tmp);
    }

    // set the pointer to NULL again (rather style)
    tmp = NULL;

    // return error code
    return -1;
  }

  // assign the new space to the passed pointer of a pointer 
  // (the first pointer is kept in the parent function, we 
  // change the target to where this 1. pointer points to
  // and make it point to another 2. pointer
  *str = tmp;
  
  // also we change the size to the new size - this is only the value, 
  // it will continue to be stored at the same address (so we passed 
  // a "pointer" (only one!)
  *str_size += how_much_more;

  return 0;
}


/*
  allocates a char*, inits it and sets its size
//*/
int init_text(char** pResult, char* pText, unsigned int* pSize)
{
  // checks if pText and pSize are NULL -> bogus, return -1
  if(pText == NULL) return -1;
  if(pSize == NULL) return -1;

  // set the size to the size of pText
  *pSize = (strlen(pText) + 1);

  // allocation
  if(NULL == (*pResult = calloc(*pSize, sizeof(**pResult)))){
    perror("allocation failed");    
    return -1;
  }

  // init pResult with the passed text
  strncpy(*pResult, pText, *pSize);

  return 0;
}


/*
  initializes the values
//*/
int init_value(int** pResult, const int value)
{
  // allocate a single value
  if(NULL == (*pResult = malloc(sizeof(**pResult)))){
    perror("allocation failed");
    exit(EXIT_FAILURE);
  }

  // init pResult
  **pResult = value;

  return 0;
}


/*
  allocates memory and copies into a new char*, if it works out, 
  it changes the old size value to the new value

  further the function demonstrates some bogus changes that could
  be done, generally it wouldn't be necessary always to check 
  everything possible, though.
  there are critical checks and minor ones, sometimes it depends on 
  various factors to decide which checks are crucial and sometimes 
  it should just be the common way to do it, e.g. in case of an 
  allocation

  COPY_SIZE = number of the char's to copy, starting from index '0'
  pOld_size = size of the old string inclusive the '\0'
  NEW_SIZE = size of the new string inclusive '\0'
//*/
int trim_char(char** pStr, const unsigned int COPY_SIZE, unsigned int *pOld_size, const unsigned int NEW_SIZE)
{
  // check passed arguments for valid values
  if(*pStr == NULL) return -1;
  if(COPY_SIZE > *pOld_size) return -1;
  if(COPY_SIZE > NEW_SIZE) return -1;

  // define pNewStr
  char* pNewStr = NULL;

  // allocation pNewStr
  if(NULL == (pNewStr = calloc(NEW_SIZE, sizeof(*pNewStr)))){
    perror("allocation failed");
    return -1;
  }

  // initialization pNewStr - this also can fail (but generally not 
  // is that critical like an allocation, though here it will be proved 
  // if it worked ;-)
  if(NULL == (pNewStr = strncpy(pNewStr, *pStr, COPY_SIZE))){
    // if this failed the allocated memory HAS TO BE FREED!!!!!
    if(pNewStr != NULL){
      free(pNewStr);
    }
    return -1;
  }

  // check if this pointer is already used - this works ONLY if the pointer 
  // points to allocated memory. If it points to static memory this gives 
  // a Signal 11 (SIGSEGV, SIGnal SEGmentation Violation) 
  if(NULL != *pStr){
    free(*pStr);
  }

  // set pStr to pNewStr
  *pStr = pNewStr;

  // check if '\0' is set
  if((*pStr)[NEW_SIZE-1] != '\0'){ 
    (*pStr)[NEW_SIZE-1] = '\0';
  }

  // new size is now the current size
  *pOld_size = NEW_SIZE;

  return 0;
}


/*
  free the space in a subfunction
//*/
void freeAll(char** pText)
{
  if(*pText != NULL){ 
    free(*pText);
  }
}



int main(int argc, char** argv)
{
  printf("allocation demo\n");

  char* pText = NULL;
  unsigned int text_size = 0;

  // init text - the first number is the return value of the function
  printf("%d: init text\n", init_text(&pText, "foo bar", &text_size));
  printf("\t\"%s\",\t\tsize: %02d - strlen: %02d + 1\n", pText, text_size, strlen(pText));
  printf("\n");

  // realloc space and append text
  unsigned int add_size = strlen(" blub ");
  printf("%d: realloc some space\n", get_more_space(&pText, &text_size, add_size));
  pText = strncat(pText, " blub ", 1 + strlen(" blub "));
  printf("\t\"%s\",\tsize: %02d - strlen: %02d + 1\n", pText, text_size, strlen(pText));
  printf("\n");

  // trim text
  unsigned int trim_size;
  for(trim_size = strlen(pText); (trim_size > 0) && ((pText[trim_size] == ' ') || (pText[trim_size] == '\0')); --trim_size);

  // correct counter (for loop counting prob)
  ++trim_size;

  // give one more due to '\0' in trim_size
  printf("%d: trim text\n", trim_char(&pText, trim_size+1, &text_size, trim_size+1));
  printf("\t\"%s\",\t\tsize: %02d - strlen: %02d + 1\n", pText, text_size, strlen(pText));
  printf("\n");

  // do something with malloc()
  int* pVal = NULL;
  printf("%d: use malloc\n", init_value(&pVal, 7));
  printf("\tResult: %d\n", *pVal);
  printf("\n");

  puts("READY.");
  freeAll(&pText);
  return EXIT_SUCCESS;
}



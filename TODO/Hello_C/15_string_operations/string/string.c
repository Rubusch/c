// string.c
/*
  string operations in C

  demonstrates some basic string operations:

    strncpy(); - string copy, params are: dest, source, size_of_source

    strncmp(); - string compare (returns 0 if equal!)

    strlen();  - string length (without '\0')

    strncat(); - string concatenation (target has to be valid
                 memory -> allocated or static!)

    strtok();  - separating a string by a token, e.g. ' ' so we gain separated
                 words




  while - break - trick

  The do-while() is executed once, even if the expression in the
  while() parenthesis is false.

  We can take advantage out of this by putting cleaning up work
  after the while, and in case something bad happened, instead
  of an exit() we do a break;

  Example to use this trick:
  when allocating in several places - if e.g. the second allocation fails
  and we would do an exit, the space for the first allocation would still
  stay allocated - you can't rely on the operating system cleaning up
  allocated memory for you. In general it does, but there is no garantee
  to that!!! Each program that allocates space, has also to free this space
  after use, strange errors could be a consequence, if not!!!

  So here we do a break, instead of an exit, to jump directly to the end
  of the do/while and there having the free()'s, to do the clean up.

  A further example of this is the usage in socket implementation.

  An alternative (quicker) is the use of labels and goto - this is the only
  use of goto, BUT goto is the worst style possible in C. Though there are
  cases, especially in the core implementation of an operating system where
  it is used in cases like "free ressource asap or the whole system will crash".



  reallocation by pointers for lists of string arrays

  here reallocation is done using a tmp pointer, new space is to be allocated
  for the temp pointer, it will be assigned to the "list pointer" and the temp
  pointer is to be set to NULL or another space again.



  known issues..

  strtok() has some issues and sometimes doesn't compile well - e.g.
  'a' will fail to be recognized -> recompile 2x!
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXTSIZ 256
#define TEXT                                                                   \
  "All work and no play makes Jack a dull boy, all play and no work make "     \
  "Jack a mere toy!"
#define PATTERN "Jack"

int check_text_for_pattern(char *, const unsigned long int, char *,
                           const unsigned long int);


int main()
{
  // definition
  char *text = NULL;
  int text_size = 0;
  char *pattern = NULL;
  int pattern_size = 0;

  /*
    while/break trick
  //*/
  do {
    puts("pattern matching");

    /*
      variable: text
    //*/

    // alloc and init the text
    if (NULL == (text = calloc(TEXTSIZ, sizeof(*text)))) {
      perror("allocation for *text failed");
      break;
    }

    // init to '\0'
    memset(text, '\0', TEXTSIZ);

    // determine the size of the init string to set to text
    text_size = 1 + strlen(TEXT);

    // set string to text
    strncpy(text, TEXT, text_size);


    /*
      variabel: pattern
    //*/

    // allocation
    if (NULL == (pattern = calloc((1 + strlen(PATTERN)), sizeof(*pattern)))) {
      perror("allocation for *pattern failed");
      break;
    }

    // reset to '\0'
    memset(pattern, '\0', (1 + strlen(PATTERN)));

    // determine size of pattern
    pattern_size = 1 + strlen(PATTERN);

    // set pattern
    strncpy(pattern, PATTERN, pattern_size);

    printf("\ntext:\n\"%s\"\n\npattern:\n\"%s\"\n", text, pattern);

    // check if lines contained PATTERN
    int result = 0;
    puts("\nsearch for the pattern...");
    if (1 == (result = check_text_for_pattern(text, text_size, pattern,
                                              pattern_size))) {
      printf("\n -> Contains the pattern: \"%s\"\n\n", pattern);
    } else {
      printf("\n -> Doesn't contain the pattern: %d\n\n", result);
    }

  } while (0);

  // clean up: free
  if (NULL != text)
    free(text);
  if (NULL != pattern)
    free(pattern);

  puts("READY.");
  exit(EXIT_SUCCESS);
}


/*
  checks if a pattern is contained in the passed string

  if it is contained it returns 1, else returns 0, in case of error -1
//*/
int check_text_for_pattern(char *text, const unsigned long int text_size,
                           char *pattern, const unsigned long int pattern_size)
{


  /*****  checks  *****/


  if (pattern == NULL)
    return 0;
  if (text == NULL)
    return 0;


  /*****  init  *****/

  unsigned int cnt = 0;
  char *word = NULL;
  char token = ' ';
  char **ppWordList_temp = NULL;

  // the text temp copy, it has one character more, for the added '\n'
  char text_temp[text_size + 1];

  // allocate space for each word
  char **ppWordList = NULL;
  unsigned int wordListSize = 0;

  // the result value
  int result = 0;

  // copy the text to a text_temp variable, without '\0', to add '\n\0'
  // afterwards
  strncpy(text_temp, text, text_size - 1);
  strncat(text_temp, "\n", 2);

  // call strtok() the first time and init it with pointer to text_temp
  if (NULL == (word = strtok(text_temp, &token))) {
    return 0;
  }


  /*****  separating the string into a list of words  *****/


  /*
    strtok() part 1: init by applying to the first element

    allocate space for the pointer of the first element in the list,
    increment the element counter,
    then allocate space for the size of the first element itself
    and initialize the element
  //*/
  if (NULL == (ppWordList = malloc(sizeof(*ppWordList)))) {
    perror("allocation of *ppWordList failed");
    return -1;
  }
  ++wordListSize;
  if (NULL ==
      (ppWordList[wordListSize - 1] =
           calloc((1 + strlen(word)), sizeof(*ppWordList[wordListSize - 1])))) {
    perror("allocation of *ppWordList[wordListSize -1] failed");
    return -1;
  }
  strncpy(ppWordList[wordListSize - 1], word, (1 + strlen(word)));


  /*
    strtok() part 2: continueing in a loop

    strtok() needs to be initialized with a value the first time being used
    and then called with "NULL" within a loop (the loop below)
  //*/
  do {
    // reset the text_temp list pointer
    ppWordList_temp = NULL;

    // use strtok() this time with NULL, because already inited above
    if (NULL == (word = strtok(NULL, &token))) {
      break;
    }

    /*
      ppWordList_temp re-allocation

      reallocate space for one new pointer in the list
      therefore allocate a temp pointer,
      assign the addresses of the list to the temp pointer
      and assign the ppWordList_temp's address to the ppWordList pointer
    //*/

    // 1. allocate space for wordListSize + 1 pointers to elements in
    // ppWordList_temp
    if (NULL == (ppWordList_temp =
                     calloc((wordListSize + 1), sizeof(*ppWordList_temp)))) {
      perror("allocation of *ppWordList_temp failed");
      result = -1;
      break;
    }

    // 2. copy by pointer, ppWordList_temp points now to the same address!
    unsigned int cnt;
    for (cnt = 0; cnt < wordListSize; ++cnt) {
      ppWordList_temp[cnt] = ppWordList[cnt];
    }

    /*
       3. allocate memory for elements refered by each pointer already allocated
    in "1." (index: wordListSize, still not incremented!)

       only the last element will be put into new allocated space,
    //*/
    if (NULL == (ppWordList_temp[wordListSize] =
                     calloc((1 + strlen(word)),
                            sizeof(*ppWordList_temp[wordListSize])))) {
      perror("allocation of ppWordList_temp[wordListSize] failed");
      result = -1;
      break;
    }
    strncpy(ppWordList_temp[wordListSize], word, 1 + strlen(word));

    /*
      4. back setting of the temp list to the original
    //*/
    ppWordList = ppWordList_temp;

    /*
      5. incrementing the size of the list
    //*/
    ++wordListSize;
  } while (word);


  /*****  finding the pattern in the list of words  *****/


  // and search the list for the pattern
  if (-1 != result) {
    for (cnt = 0; cnt < wordListSize; ++cnt) {
      printf("\t%d. compare: pattern \"%s\" with \"%s\" - %d token compared\n",
             cnt, pattern, ppWordList[cnt],
             (1 + strlen(ppWordList[cnt]))); // XXX
      if (0 ==
          strncmp(pattern, ppWordList[cnt], (1 + strlen(ppWordList[cnt])))) {
        result = 1;
        break;
      }
    }
  }

  // free allocated space
  for (cnt = 0; cnt < wordListSize; ++cnt) {
    if (NULL != ppWordList[cnt]) {
      free(ppWordList[cnt]);
      ppWordList[cnt] = NULL;
    }
  }
  if (NULL != ppWordList) {
    free(ppWordList);
    ppWordList = NULL;
  }
  if (NULL != ppWordList_temp) {
    free(ppWordList_temp);
    ppWordList_temp = NULL;
  }

  // not found -> 0
  return result;
}

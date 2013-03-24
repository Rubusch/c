// Strtok.c

/*
  strtok() is not threadsafe!!!

  Separate a string by tokens and search for a pattern contained in that string.

  Error (hidden token!):
  user@machine:/mnt/usbstick/programming/TODO_Strtok$ make
  gcc -c -O -g -Wall -std=c99 Strtok.c
  Strtok.c:22: error: expected identifier or ’¡Æ(’¡Ç before ’¡Æ__extension__’¡Ç
  Strtok.c:22: error: expected identifier or ’¡Æ(’¡Ç before ’¡Æ)’¡Ç token
  make: ** [Strtok.o] Erro 1
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXT_SIZE 4
#define PATTERN "Jill"

extern char* strcpy(char*, const char*);
extern size_t strlen(const char*); 
extern char* strtok(char*, const char*);
extern int strcmp(const char*, const char*);
extern char* strcat(char*, const char*);


int check_line_for_pattern( char* line, const unsigned long int LINE_SIZE, 
			    char* pattern, const unsigned long int PATTERN_SIZE);


int main(int argc, char** argv)
{
  // init a char** with some text
  printf("Strtok - separate a string into tokens\n\ninit a char** with some text:\n");
  char** text; 
  if( (text = malloc(sizeof(char*)*4)) == NULL) return EXIT_FAILURE;
  if( (text[0] = malloc(sizeof(char) * 
			(strlen("Jack and Jill went up the hill to fetch a pail of water") + 1))) == NULL) return -1;
  strcpy(text[0], "Jack and Jill went up the hill to fetch a pail of water");
  if( (text[1] = malloc(sizeof(char) * 
			(strlen("Jack fell down and broke his crown and Jill came tumbling after.") + 1))) == NULL) return -1;
  strcpy(text[1], "Jack fell down and broke his crown and Jill came tumbling after.");
  if( (text[2] = malloc(sizeof(char) * 
			(strlen("Up got Jack, and home did trot as fast as he could caper") + 1))) == NULL) return -1;
  strcpy(text[2], "Up got Jack, and home did trot as fast as he could caper");
  if( (text[3] = malloc(sizeof(char) * 
			(strlen("He went to bed and bound his head with vinegar and brown paper.") + 1))) == NULL) return -1;
  strcpy(text[3], "He went to bed and bound his head with vinegar and brown paper.");
  
  // output
  unsigned int cnt;
  for(cnt = 0; cnt < TEXT_SIZE; ++cnt) printf("%i. line: \n#%s#\n", cnt, text[cnt]);

  // set a pattern
  char* pattern;
  if( (pattern = malloc(sizeof(PATTERN))) == NULL) return EXIT_FAILURE;
  strcpy(pattern, PATTERN);
  printf( "\nset a pattern: #%s#\n\n", pattern);

  // test which line contains the pattern
  printf( "test which line contains the pattern:\n\n");
  for(cnt = 0; cnt < TEXT_SIZE; ++cnt){
    printf("%d.line - CHECKING: \n\"%s\"\n", cnt, text[cnt]); // DEBUG
    if(check_line_for_pattern(text[cnt], strlen(text[cnt]), pattern, strlen(pattern))){
      printf( "\tContains the pattern: \"%s\"\n\n", pattern); 
    }else{
      printf("\tNothing!\n\n"); // DEBUG
    }
  }
  
  printf("READY.\n");
    
  return EXIT_SUCCESS;
}


/*
  reads a line and checks if it contains a certain pattern / word
  returns 1 in case of occurance, 0 in case of no occurance or error

  Works, but only reads in
//*/
int check_line_for_pattern( char* line, const unsigned long int LINE_SIZE, char* pattern, const unsigned long int PATTERN_SIZE)
{
  if(pattern == NULL) return 0;
  if(line == NULL) return 0;

  char* word = NULL;
  char token = ' ';
  char** ppWordListTemp = NULL;
  char temp[LINE_SIZE+1];
  char** ppWordList = NULL; // allocate space for each word XXX
  unsigned int wordListSize = 0;
       
  // processing each line: strtok into pieces by ' ', append to a list
  strcpy(temp, line);
  strcat(temp, "\n"); // TODO: check appending an '\n'

  // init strtok() with pointer to the '\0'-ed temp
  if( (word = strtok(temp, &token)) == NULL) return 0; 
    
  // allocate space for one new pointer in the list, allocate space by the size of 
  // word for this pointer, append to the list, and increment counter
  if( (ppWordList = (char**) malloc(sizeof(char*))) == NULL) return 0; 
  // TODO: better sizeof(*ppWordList) ?
  ++wordListSize; 

  if( (ppWordList[wordListSize - 1] = malloc( sizeof(char) * (strlen(word) + 1))) == NULL) return 0;
  strcpy( ppWordList[wordListSize - 1], word); // GDB - check ppWordList, wordListSize, word
  do{
    // reset the temp list pointer
    ppWordListTemp = NULL;

    // use strtok() with NULL, because already inited
    if( (word = strtok(NULL, &token)) == NULL) break; 

    // reallocate space for one new pointer in the list 
    // therefore allocate a temp pointer, assign the addresses of the list to this and 
    // assign the ppWordListTemp's address to the ppWordList pointer 
    if( (ppWordListTemp = malloc((wordListSize+1) * sizeof(char*))) == NULL) return 0;

    unsigned int cnt;
    for(cnt = 0; cnt < wordListSize; ++cnt) ppWordListTemp[cnt] = ppWordList[cnt];
    // ppWordListTemp points to the same address!!    

    // allocate ppWordListTemp[wordListSize] for one new element (index: wordListSize, still not incremented!)
    // the last element will be put into new allocated space
    if( (ppWordListTemp[wordListSize] = malloc( strlen(word) * (sizeof(char) + 1))) == NULL);
    strcpy(ppWordListTemp[wordListSize], word);
    ppWordList = ppWordListTemp;
    ++wordListSize; 
  }while(word);

  // and search the list for the pattern 
  unsigned int cnt;
  for(cnt = 0; cnt < wordListSize; ++cnt){
    printf("DEBUG: %i. line\tpattern:\"%s\" - \"%s\"\n", cnt, pattern, ppWordList[cnt]); // DEBUG
    // BETTER: use improved implementation of strncmp()!!!
    if(0 == strcmp(pattern, ppWordList[cnt])){ 
      for(cnt = 0; cnt < wordListSize; ++cnt) free(ppWordList[cnt]);
      free(ppWordList);  
      return 1;
    }
  }
  
  // free allocated space
  for(cnt = 0; cnt < wordListSize; ++cnt) free(ppWordList[cnt]);
  free(ppWordList);
  free(ppWordListTemp);

  return 0;
}


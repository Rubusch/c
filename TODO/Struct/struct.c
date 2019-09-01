// struct
/*
  dyn pointers and structs
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

typedef struct text_etikett {
  unsigned int text_size;
  char* szText;
} text_etikett_t;


int text_alloc(struct text_etikett** ppText)
{
  if (NULL == (*ppText = malloc(sizeof(**ppText)))) return -1;
  return 0;
}


// NO NEED FOR A DOUBLED POINTER FOR ACCESSING ELEMENTS OF A STRUCT (ALLOCATION, FREE, ADDRESS ASSIGNMENT)!!!
int text_init(struct text_etikett* pText, char* text, const unsigned int TXT_SIZE)
{
  if (NULL == pText) return -1;

  // init text
  if (NULL == (pText->szText = calloc(TXT_SIZE, sizeof(*pText->szText)))) return -1;
  strcpy(pText->szText, text);

  // text size
  pText->text_size = TXT_SIZE;

  return 0;
}


int free_text(struct text_etikett** ppText)
{
  if (NULL == *ppText) return -1;

  free((*ppText)->szText);
  free(*ppText);
  *ppText = NULL;

  return 0;
}


int main(int argc, char** argv)
{
  printf("init \'struct text_etikett\'\n");
  struct text_etikett* text01 = NULL;
  if(text_alloc(&text01) == -1) return EXIT_FAILURE;

  printf("set values\n");
  text_init(text01, "foobar", (strlen("foobar") + 1));

  printf("show:\t\"%s\"\t\'%d\' - strlen: \'%li\' + 1\n", text01->szText, text01->text_size, strlen(text01->szText));


  printf("\n\n *** Output using the typedefed struct ***\n\n");
  printf("init \'typedef\'ed struct\n");
  text_etikett_t* text02 = NULL;
  if (-1 == text_alloc(&text02)) return EXIT_FAILURE;

  printf("set values\n");
  text_init(text02, "almost all", (strlen("almost all") + 1));

  printf("show:\t\"%s\"\t\'%d\' - strlen: \'%li\' + 1\n", text02->szText, text02->text_size, strlen(text02->szText));


  printf("\n\nfree_text()\n");
  free_text(&text01); assert(NULL == text01);
  free_text(&text02); assert(NULL == text02);

  printf("READY.\n");

  return EXIT_SUCCESS;
}

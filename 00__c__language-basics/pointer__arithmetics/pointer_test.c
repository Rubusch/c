// pointer_test.c
/*
  demonstrates strlen() and sizeof(a)/sizeof(char)
  demonstrates the use of pointers and double pointers on chars
//*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void show(char arr[44], char *pArr, char **ppArr)
{
  printf("\n\n");
  printf("\n========show()===start=================\n\n\n");

  //*
  printf("\n\n");
  printf(
      "~~~~ arr[] ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
  printf("arr:\t\t\t\t\"%s\"\n", arr);
  printf("\n");
  //  printf("sizeof(arr)/sizeof(char):\t%li\n", sizeof(arr)/sizeof(char)); //
  //  ATTENTION! won't compile, is a ptr not an array []!
  printf("strlen(arr):\t\t\t%li plus 1 token \'\\0\'\n", strlen(arr));
  printf("\n");
  printf("*arr:\t\t\t\t%i\t\t\t\t\t\tcontent of first pointer element '%c' "
         "which is as int '%i'\n",
         *arr, ( char )*arr, *arr);
  printf("arr:\t\t\t\t\"%s\"\tprintf interprets a string, normally address of "
         "first character\n",
         arr);
  printf("arr:\t\t\t\t%lX\t\t\t\t\taddress of the first character, same as "
         "'&arr'\n",
         ( unsigned long )arr);
  printf("&arr:\t\t\t\t%lX\t\t\t\t\taddress of the pointer (for array "
         "identical with content address)\n",
         ( unsigned long )&arr);
  printf("\t[ *&arr or &*arr:\t\"%s\"\tsame as 'arr' ]\n", &*arr);

  //*
  printf("\n\n");
  printf(
      "~~~~ *pArr ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

  printf("*pArr:\t\t\t\t\"%s\"\n", pArr);
  printf("\n");
  printf(
      "sizeof(*pArr)/sizeof(char):\t%li\n",
      sizeof(*pArr) /
          sizeof(char)); // doesn't work - due to being a *ptr and no arr[]!!!
  printf("strlen(pArr):\t\t\t%li plus 1 token \'\\0\'\n",
         strlen(pArr)); // only this works with *ptr's!!!
  printf("\n");
  printf("*pArr:\t\t\t\t%i\t\t\t\t\t\tcontent of first pointer element '%c' "
         "which is as int '%i'\n",
         *pArr, ( char )*pArr, *pArr);
  printf("pArr:\t\t\t\t\"%s\"\tprintf interprets a string, normally address of "
         "first character\n",
         pArr);
  printf("pArr:\t\t\t\t%lX\t\t\t\t\taddress of the first character, same as "
         "'&pArr'\n",
         ( unsigned long )pArr);
  printf("&pArr:\t\t\t\t%lX\t\t\t\t\taddress of the pointer\n",
         ( unsigned long )&pArr);
  printf("\t[ *&pArr or &*pArr:\t\"%s\"\tsame as 'pArr' ]\n", &*pArr);

  //*
  printf("\n\n");
  printf(
      "~~~~ **ppArr ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

  printf("**ppArr:\t\t\t\"%s\"\n", *ppArr);
  printf("\n");
  printf(
      "sizeof(**ppArr)/sizeof(char):\t%li\n",
      sizeof(**ppArr) /
          sizeof(char)); // doesn't work - due to being a *ptr and no arr[]!!!
  printf("strlen(*ppArr):\t\t\t%li plus 1 token \'\\0\'\n",
         strlen(*ppArr)); // only this works with *ptr's!!!
  printf("\n");
  printf("**ppArr:\t\t\t%i\t\t\t\t\t\tcontent of the pointer to char, '%c' "
         "which is as int '%i'\n",
         **ppArr, ( char )**ppArr, **ppArr);
  printf("*ppArr:\t\t\t\t\"%s\"\tprintf interprets a string, normally address "
         "of first character\n",
         *ppArr);
  printf("*ppArr:\t\t\t\t%lX\t\t\t\t\taddress of the first character\n",
         ( unsigned long )*ppArr);
  printf("ppArr:\t\t\t\t%lX\t\t\t\t\taddress of the char pointer pointed to by "
         "pointer\n",
         ( unsigned long )ppArr);
  printf("&ppArr:\t\t\t\t%lX\t\t\t\t\taddress of the pointer itself (pointing "
         "to the char pointer)\n",
         ( unsigned long )&ppArr);
  printf("\t[ &*ppArr:\t\t%lX\t\t\t\t\tsame as 'ppArr' ]\n",
         ( unsigned long )&*ppArr);
  printf("\t[ &**ppArr:\t\t\"%s\"\tsame as '*ppArr' ]\n", &**ppArr);
  //*/
  printf("\n\n");
  printf("\n========show()===end=====================\n\n\n");
}


int main()
{
  printf("\t MAIN START\n");
  char arr[] = "All work and no play makes Jack a dull boy.";

  //*
  printf("\n\n");
  printf(
      "~~~~ arr[] = \"...\"; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
  printf("arr:\t\t\t\t\"%s\"\n", arr);
  printf("\n");
  printf("sizeof(arr)/sizeof(char):\t%li\n", sizeof(arr) / sizeof(char));
  printf("strlen(arr):\t\t\t%li plus 1 token \'\\0\'\n", strlen(arr));
  printf("\n");
  printf("*arr:\t\t\t\t%i\t\t\t\t\t\tcontent of first pointer element '%c' "
         "which is as int '%i'\n",
         *arr, ( char )*arr, *arr);
  printf("arr:\t\t\t\t\"%s\"\tprintf interprets a string, normally address of "
         "first character\n",
         arr);
  printf("arr:\t\t\t\t%lX\t\t\t\t\taddress of the first character, same as "
         "'&arr'\n",
         ( unsigned long )arr);
  printf("&arr:\t\t\t\t%lX\t\t\t\t\taddress of the pointer (for array "
         "identical with content address)\n",
         ( unsigned long )&arr);
  printf("\t[ *&arr or &*arr:\t\"%s\"\tsame as 'arr' ]\n", &*arr);

  //*
  printf("\n\n");
  printf(
      "~~~~ *pArr = malloc(); strncpy(...); ~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

  int arr_len = sizeof(arr) / sizeof(char);
  char *pArr = NULL;
  pArr = malloc(arr_len);
  strncpy(pArr, arr, strlen(arr));

  printf("*pArr:\t\t\t\t\"%s\"\n", pArr);
  printf("\n");
  printf(
      "sizeof(*pArr)/sizeof(char):\t%li\n",
      sizeof(*pArr) /
          sizeof(char)); // doesn't work - due to being a *ptr and no arr[]!!!
  printf("strlen(pArr):\t\t\t%li plus 1 token \'\\0\'\n",
         strlen(pArr)); // only this works with *ptr's!!!
  printf("\n");
  printf("*pArr:\t\t\t\t%i\t\t\t\t\t\tcontent of first pointer element '%c' "
         "which is as int '%i'\n",
         *pArr, ( char )*pArr, *pArr);
  printf("pArr:\t\t\t\t\"%s\"\tprintf interprets a string, normally address of "
         "first character\n",
         pArr);
  printf("pArr:\t\t\t\t%lX\t\t\t\t\taddress of the first character, same as "
         "'&pArr'\n",
         ( unsigned long )pArr);
  printf("&pArr:\t\t\t\t%lX\t\t\t\t\taddress of the pointer\n",
         ( unsigned long )&pArr);
  printf("\t[ *&pArr or &*pArr:\t\"%s\"\tsame as 'pArr' ]\n", &*pArr);

  //*
  printf("\n\n");
  printf(
      "~~~~ **ppArr = malloc(); *ppArr = malloc(); strncpy() ~~~~~~~~~~\n\n");

  char **ppArr = NULL;
  ppArr = malloc(sizeof(long int));
  *ppArr = malloc(arr_len);
  strncpy(*ppArr, pArr, strlen(arr));

  printf("**ppArr:\t\t\t\"%s\"\n", *ppArr);
  printf("\n");
  printf(
      "sizeof(**ppArr)/sizeof(char):\t%li\n",
      sizeof(**ppArr) /
          sizeof(char)); // doesn't work - due to being a *ptr and no arr[]!!!
  printf("strlen(*ppArr):\t\t\t%li plus 1 token \'\\0\'\n",
         strlen(*ppArr)); // only this works with *ptr's!!!
  printf("\n");
  printf("**ppArr:\t\t\t%i\t\t\t\t\t\tcontent of the pointer to char, '%c' "
         "which is as int '%i'\n",
         **ppArr, ( char )**ppArr, **ppArr);
  printf("*ppArr:\t\t\t\t\"%s\"\tprintf interprets a string, normally address "
         "of first character\n",
         *ppArr);
  printf("*ppArr:\t\t\t\t%lX\t\t\t\t\taddress of the first character\n",
         ( unsigned long )*ppArr);
  printf("ppArr:\t\t\t\t%lX\t\t\t\t\taddress of the char pointer pointed to by "
         "pointer\n",
         ( unsigned long )ppArr);
  printf("&ppArr:\t\t\t\t%lX\t\t\t\t\taddress of the pointer itself (pointing "
         "to the char pointer)\n",
         ( unsigned long )&ppArr);
  printf("\t[ &*ppArr:\t\t%lX\t\t\t\t\tsame as 'ppArr' ]\n",
         ( unsigned long )&*ppArr);
  printf("\t[ &**ppArr:\t\t\"%s\"\tsame as '*ppArr' ]\n", &**ppArr);
  //*/

  //*
  printf("\n\n\n\n\n## 1) show(&arr, &arr, &arr) "
         "########################\n\n\n\n\n");
  printf("Doesn't work - a variable can not be referenced as pointer to "
         "pointer\n");
  //*/

  //*
  printf("\n\n\n\n\n## 2) show(pArr, pArr, &pArr) "
         "#######################\n\n\n\n\n");
  show(pArr, pArr, &pArr);
  //*/

  //*
  printf("\n\n## 3) show(*ppArr, *ppArr, ppArr) ###################\n");
  show(*ppArr, *ppArr, ppArr);
  //*/

  return EXIT_SUCCESS;
}

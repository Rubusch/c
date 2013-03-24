// pointer_test.c
/*
  demonstrates strlen() and sizeof(a)/sizeof(char)
  demonstrates the use of pointers and double pointers on chars
//*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int show_length(char arr[44], char *pArr, char **ppArr){
  printf("\n========show_length()===start=================\n\n\n");

  printf("~~~~~arr[]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
  printf("func()::arr[]: '%s\'\n", arr);
  printf("\n");
  printf("sizeof(arr)/sizeof(char):\t%i\n", sizeof(arr)/sizeof(char));
  printf("strlen(arr):\t\t\t%i plus 1 token \'\\0\'\n", strlen(arr));
  printf("\n");
  printf("func()::arr:\t\t%i\taddress of the target\n", arr);
  printf("&func()::arr:\t\t%i\taddress of the poitner\n", &arr);
  printf("*func()::arr:\t\t%i\tcontent of the pointer as int\n", *arr);
  printf("&*func()::arr:\t\t%i\taddress of the target of the pointer\n", &*arr);
  printf("\n");
  printf("conclusion:\taddress of target == address of pointer\n"); // conclusion

  printf("~~~~~*pArr~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

  printf("*func()::pArr: '%s\'\n", pArr);      
  printf("\n");
  printf("sizeof(pArr)/sizeof(char):\t%i\n", sizeof(pArr)/sizeof(char));   // doesn't work - due to being a *ptr and no arr[]!!!
  printf("strlen(pArr):\t\t\t%i plus 1 token \'\\0\'\n", strlen(pArr));    // only this works with *ptr's!!!
  printf("\n");
  printf("func()::pArr:\t\t%i\taddress of the target\n", pArr);
  printf("&func()::pArr:\t\t%i\taddress of the pointer\n", &pArr);
  printf("*func()::pArr:\t\t%i\tcontent of the pointer as int\n", *pArr);
  printf("&*func()::pArr:\t\t%i\taddress of the target of the pointer\n", &*pArr);
  printf("\n");
  printf("conclusion:\taddress of pointer != address of target\n");
  printf("\t\tdoesn't work - due to being a *ptr and no arr[]!!!\n");
  printf("\t\tonly this works with *ptr's!!!\n");

  printf("~~~~~**ppArr~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

  printf("**func()::ppArr: '%s\'\n", *ppArr);
  printf("\n");
  printf("sizeof(*ppArr)/sizeof(char):\t%i\n", sizeof(*ppArr)/sizeof(char)); // doesn't work - due to being a *ptr and no arr[]!!!
  printf("strlen(*ppArr):\t\t\t%i plus 1 token \'\\0\'\n", strlen(*ppArr));  // only this works with *ptr's!!!
  printf("\n");
  printf("func()::ppArr:\t\t%i\taddress of the char pointer\n", ppArr);
  printf("&func()::ppArr:\t\t%i\taddress of the pointer to long int\n", &ppArr);
  printf("*func()::ppArr:\t\t%i\tcontent of the long int pointer,\n\t\t\taddress of the pointer to char\n", *ppArr);
  printf("&*func()::ppArr:\t%i\taddress of the char pointer\n", &*ppArr);
  //
  printf("**func()::ppArr:\t%i\tcontent of the pointer to char, \'A\' as int\n", **ppArr);
  printf("&**func()::ppArr:\t%i\taddress of the content of the\n\t\t\tpointer to char referenced by the\n\t\t\tpointer to long int\n", &**ppArr);
  printf("\n");
  printf("conclusion:\ntype:\t\tchar\t\t\t*char\t\t*long int\n\ncontent:\t**ppArr\t\t\t<-**- *ppArr\t<-*- ppArr\n\naddress:\t&**ppArr, *ppArr\t&*ppArr, ppArr\t&ppArr\n");
  //*/
  printf("\n========show_length()===end=====================\n\n\n");
};


int main()
{
  printf("\t MAIN START\n");
  char arr[] = "All work and no play makes Jack a dull boy.";

  printf("~~~~~arr[]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
  printf("arr[]: '%s\'\n", arr);
  printf("\n");
  printf("sizeof(arr)/sizeof(char):\t%i\n", sizeof(arr)/sizeof(char));
  printf("strlen(arr):\t\t\t%i plus 1 token \'\\0\'\n", strlen(arr));
  printf("\n");
  printf("arr:\t\t%i\taddress of the target\n", arr);
  printf("&arr:\t\t%i\taddress of the poitner\n", &arr);
  printf("*arr:\t\t%i\tcontent of the pointer as int\n", *arr);
  printf("&*arr:\t\t%i\taddress of the target of the pointer\n", &*arr);

  printf("~~~~~*pArr~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

  int arr_len = sizeof(arr)/sizeof(char);
  char *pArr=NULL;
  pArr = malloc(arr_len);
  strncpy(pArr, arr, strlen(arr));

  printf("*pArr: '%s\'\n", pArr);
  printf("\n");
  printf("sizeof(pArr)/sizeof(char):\t%i\n", sizeof(pArr)/sizeof(char)); // doesn't work - due to being a *ptr and no arr[]!!!
  printf("strlen(pArr):\t\t\t%i plus 1 token \'\\0\'\n", strlen(pArr));  // only this works with *ptr's!!!
  printf("\n");
  printf("pArr:\t\t%i\taddress of the target\n", pArr);
  printf("&pArr:\t\t%i\taddress of the pointer\n", &pArr);
  printf("*pArr:\t\t%i\tcontent of the pointer as int\n", *pArr);
  printf("&*pArr:\t\t%i\taddress of the target of the pointer\n", &*pArr);

  printf("~~~~~**ppArr~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

  char **ppArr = NULL;
  ppArr = malloc(sizeof(long int));
  *ppArr = malloc(arr_len);
  strncpy(*ppArr,pArr, strlen(arr));

  printf("**ppArr: '%s\'\n", *ppArr);      
  printf("\n");
  printf("sizeof(*ppArr)/sizeof(char):\t%i\n", sizeof(*ppArr)/sizeof(char)); // doesn't work - due to being a *ptr and no arr[]!!!
  printf("strlen(*ppArr):\t\t\t%i plus 1 token \'\\0\'\n", strlen(*ppArr));  // only this works with *ptr's!!!
  printf("\n");
  printf("ppArr:\t\t%i\taddress of the char pointer\n", ppArr);
  printf("&ppArr:\t\t%i\taddress of the pointer to long int\n", &ppArr);
  printf("*ppArr:\t\t%i\tcontent of the long int pointer,\n\t\t\taddress of the pointer to char\n", *ppArr);
  printf("&*ppArr:\t%i\taddress of the char pointer\n", &*ppArr);
  //
  printf("**ppArr:\t%i\tcontent of the pointer to char, \'A\' as int\n", **ppArr);
  printf("&**ppArr:\t%i\taddress of the content of the \n\t\t\tpointer to char referenced by the \n\t\t\tpointer to long int\n", &**ppArr);
  //*/
  //*
  printf("\n\n\n\n\n###1#show_length(arr)#########################\n\n\n\n\n");
  //  show_length(&arr, &arr, &arr); // TODO: test
  printf("Doesn't work - a variable can not be referenced as pointer to pointer\n"); // XXX
  //*/

  //*
  printf("\n\n\n\n\n###2#show_length(pArr)#########################\n\n\n\n\n");
  show_length(pArr, pArr, &pArr);
  //*/

  printf("~~~3~show_length(ppArr)~~~~~~~~~~~~~~~~~~~~~~~~\n");
  show_length(*ppArr, *ppArr, ppArr);
  //*/

  printf("\n\nint show_length(char arr[44], char *pArr, char *ppArr)\n");
  printf("\t MAIN END\n");
  return EXIT_SUCCESS;
}


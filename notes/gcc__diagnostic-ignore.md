# gcc diagnostic push / pop and ignore warnings

Allows to ignore warnings on particular lines of code

## REFERENCE
https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html

## EXAMPLE
```
   ...
   61
   62         puts("READY.\n");
   63         return 0;
   64 }
   65
   66 // Definition of a funcion with 2-dim-array
   67 void output(char arr[][ELEMENT_LENGTH])
   68 {
   69 #pragma GCC diagnostic push
   70         // gcc will warn about the sizeof(arr) which is clearly constant,
   71         // since we have arr[][ELEMENT_LENGTH]
   72         //
   73         // this demo should demonstrate language-basics, though, so the
   74         // warning is turned off for that purpose
   75
   76         // CAUTION: here the "sizeof" of the static array is unknown - has to be
   77         // passed as param!!!
   78 #pragma GCC diagnostic ignored "-Wsizeof-array-argument"
   79         printf("\nSize of the array is: %li - FAILS!\n",
>> 80                (sizeof(arr) / ELEMENT_LENGTH));
   81 #pragma GCC diagnostic pop
   82
   83         // print out
   84         int idx;
   ...
```


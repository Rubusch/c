/*
  C11 feature test macros

  Which version of the C standard are we using? Print some information to stdout.

  This utility is based on a utility from Pennstate University.

  The following are required in the C11 Standard (mandatory macros).
    __STDC__C89C99C11
    __STDC_HOSTED__C99C11
    __STDC_VERSION__(C94)C99C11

  The following are optional in the C11 Standard (environment macros).
    __STDC_ISO_10646__C99C11
    __STDC_MB_MIGHT_NEQ_WC__C99C11
    __STDC_UTF_16__C11
    __STDC_UTF_32__C11

  The following are optional in the C11 Standard (conditional feature macros).
    __STDC_ANALYZABLE__C11
    __STDC_IEC_559__C99C11
    __STDC_IEC_559_COMPLEX__C99C11
    __STDC_LIB_EXT1__C11
    __STDC_NO_ATOMICS__C11
    __STDC_NO_COMPLEX__C11
    __STDC_NO_THREADS__C11
    __STDC_NO_VLA__C11

  The following are required in the C11 Standard (mandatory macros).
    __DATE__C89C99C11
    __FILE__C89C99C11
    __LINE__C89C99C11
    __TIME__C89C99C11


  ---
  References:

  Pennstate University Snippet, found here:
  http://www.cse.psu.edu/~deh25/cmpsc311/Lectures/Standards/CStandard/CStandard.c

  feature test macros for C11
  https://en.wikipedia.org/wiki/C11_(C_standard_revision)

  analyzability
  https://en.cppreference.com/w/c/language/analyzability

  bounds checking (_s functions)
  https://stackoverflow.com/questions/47867130/stdc-lib-ext1-availability-in-gcc-and-clang

  multithreading
  https://en.cppreference.com/w/c/thread

  atomics
  https://en.cppreference.com/w/c/atomic

  complex numbers
  https://en.cppreference.com/w/c/numeric/complex

  array and VLA
  https://en.cppreference.com/w/c/language/array
*/


#include <stdio.h>
#include <stdlib.h>


void configuration(void)
{
#ifdef __cplusplus /* C++ */
        /*
	  expected values would be 199711L for ISO/IEC 14882:1998 or 14882:2003
	*/
	printf("This is C++, version %d.\n", __cplusplus);


#elif defined(__STDC__) /* C */
	printf("This is standard C.\n");


#  if __STDC__ == 1
	printf("\tThe compilation is ISO-conforming.\n");
#  else
	printf("\tThe compilation is NOT ISO-conforming.\n");
	printf("\t__STDC__ = %d\n", __STDC__);
#  endif


#  if defined(__STDC_VERSION__)	/* we're talking about C */
#    if __STDC_VERSION__ >= 201112L
        printf("\t\tThis is C11.\n");

#    elif (__STDC_VERSION__ >= 199901L)
        printf("\t\tThis is C99.\n");
#    elif (__STDC_VERSION__ >= 199409L)
        printf("\t\tThis is C89 with amendment 1.\n");
#    else
        printf("\t\tThis is C89 without amendment 1.\n");
        printf("\t\t__STDC_VERSION__ = %ld\n", __STDC_VERSION__);
#    endif

#  else /* no __STDC_VERSION__ */
	printf("\tThis is C89. __STDC_VERSION__ is NOT defined.\n");
#  endif

#else   /* no __STDC__ */
	printf("This is NOT standard C. __STDC__ is not defined.\n");
#endif


#if defined(__STDC__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#  ifdef __STDC_HOSTED__
#    if __STDC_HOSTED__ == 1
	printf("\t\tThis is a hosted compilation.\n");
#    elif (__STDC_HOSTED__ == 0)
	printf("\t\tThis is a freestanding compilation.\n");
#    else
	printf("\t\t__STDC_HOSTED__ = %d is an unexpected value.\n", __STDC_HOSTED__);
#    endif
#  else
	printf("\t__STDC_HOSTED__ is NOT defined.\n");
	printf("\tThis should indicate hosted or freestanding compilation.\n");
#  endif


#  ifdef __STDC_ISO_10646__
	printf("\tThe wchar_t values conform to the ISO/IEC 10646 standard (Unicode) as of %ld.\n", __STDC_ISO_10646__);
#  else
	printf("\t__STDC_ISO_10646__ is NOT defined.\n");
	printf("\tThe wchar_t values are compilation-defined.\n");
#  endif


#  ifdef __STDC_MB_MIGHT_NEQ_WC__ /* added in C99 Technical Corrigendum 3 */
#    if __STDC_MB_MIGHT_NEQ_WC__ == 1
        printf("\t\tWide and multibyte characters might NOT have the same codes.\n");
#    else
        printf("\t\t__STDC_MB_MIGHT_NEQ_WC__ = %d is an unexpected value.\n", __STDC_MB_MIGHT_NEQ_WC__);
#    endif
#  else
	printf("\t__STDC_MB_MIGHT_NEQ_WC__ is NOT defined.\n");
	printf("\tWide and multibyte characters should have the same codes.\n");
#  endif


#  ifdef __STDC_IEC_559__  /* __STDC_IEC_559__ - IEC 60559 floating-point arithmetic */
#    if __STDC_IEC_559__ == 1
        printf("\t\tThe floating-point compilation conforms to Annex F (IEC 60559 standard).\n");
#    else
        printf("\t\t__STDC_IEC_559__ = %d is an unexpected value.\n", __STDC_IEC_559__);
#    endif
#  else
	printf("\t__STDC_IEC_559__ is NOT defined.\n");
	printf("\tThe floating-point compilation does NOT conform to Annex F (IEC 60559 standard).\n");
#  endif


#  ifdef __STDC_IEC_559_COMPLEX__ /* __STDC_IEC_559_COMPLEX__ - IEC 60559 compatible complex arithmetic */

#    if __STDC_IEC_559_COMPLEX__ == 1
        printf("\t\tThe complex arithmetic compilation conforms to Annex G (IEC 60559 standard).\n");
#    else
        printf("\t\t__STDC_IEC_559_COMPLEX__ = %d is an unexpected value.\n", __STDC_IEC_559_COMPLEX__);
#    endif
#  else
	printf("\t__STDC_IEC_559_COMPLEX__ is NOT defined.\n");
	printf("\tThe complex arithmetic compilation does NOT conform to Annex G (IEC 60559 standard).\n");
#  endif
#endif


#if defined(__STDC__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#  if defined(__STDC_UTF_16__)
#    if (__STDC_UTF_16__ == 1)
	printf("\t\tThe char16_t values are UTF-16 encoded.\n");
#    else
	printf("\t\t__STDC_UTF_16__ = %d is an unexpected value.\n", __STDC_UTF_16__);
#    endif
#  else
	printf("\t__STDC_UTF_16__ is NOT defined.\n");
	printf("\tThe char16_t values are compilation-defined.\n");
#  endif


#  ifdef __STDC_UTF_32__ /* */
#    if __STDC_UTF_32__ == 1
	printf("\t\tThe char32_t values are UTF-32 encoded.\n");
#    else
	printf("\t\t__STDC_UTF_32__ = %d is an unexpected value.\n", __STDC_UTF_32__);
#    endif
#  else
	printf("\t__STDC_UTF_32__ is NOT defined.\n");
	printf("\tThe char32_t values are compilation-defined.\n");
#  endif


#  ifdef __STDC_ANALYZABLE__ /* __STDC_ANALYZABLE__ - Analyzability */
	/*
	  This optional extension to the C language limits the
	  potential results of executing some forms of undefined
	  behavior, which improves the effectiveness of static
	  analysis of such programs. Analyzability is only guaranteed
	  to be enabled if the predefined macro constant
	  __STDC_ANALYZABLE__(C11) is defined by the compiler.

	  If the compiler supports analyzability, any language or
	  library construct whose behavior is undefined is further
	  classified between critical and bounded undefined behavior,
	  and the behavior of all bounded UB cases is limited as
	  specified below.
	*/
#    if __STDC_ANALYZABLE__ == 1
	printf("\t\tThe compiler conforms to the specifications in Annex L (Analyzability).\n");
#    else
	printf("\t\t__STDC_ANALYZABLE__ = %d is an unexpected value.\n", __STDC_ANALYZABLE__);
#    endif
#  else
	printf("\t__STDC_ANALYZABLE__ is NOT defined.\n");
	printf("\tThe compiler does NOT conform to the specifications in Annex L (Analyzability).\n");
#  endif


#  ifdef __STDC_LIB_EXT1__ /* __STDC_LIB_EXT1__ - Bounds-checking interfaces */
	/*
	  The whole set of 'safe' functions with the _s suffixes is
	  poorly supported. Microsoft wrote a set of functions with
	  the _s suffixes and submitted that to the C standard
	  committee to be standardized. The committee made some
	  changes (arguably out of necessity), and created a technical
	  report, TR 24731-1. A mildly modified version of the TR was
	  included as optional Annex K (normative) in the C11
	  standard, ISO/IEC 9899:2011.

	  (source: see references)
	*/

	printf("\tThe compilation supports the extensions defined in Annex K (Bounds-checking interfaces) as of %ld.\n", __STDC_LIB_EXT1__);
#  else
	printf("\t__STDC_LIB_EXT1__ is NOT defined.\n");
	printf("\tThe compilation does NOT support the extensions defined in Annex K (Bounds-checking interfaces).\n");
#  endif


#  ifdef __STDC_NO_ATOMICS__ /* __STDC_NO_ATOMICS__ - Atomic primitives and types */
	/*
	  (<stdatomic.h> and the _Atomic type qualifier)

	  If the macro constant __STDC_NO_ATOMICS__(C11) is defined by
	  the compiler, the header <stdatomic.h>, the keyword _Atomic,
	  and all of the names listed there are not provided.
	*/

#    if __STDC_NO_ATOMICS__ == 1
	printf("\t\tThe compilation does NOT support atomic types.\n");
#    else
	printf("\t\t__STDC_NO_ATOMICS__ = %d is an unexpected value.\n", __STDC_NO_ATOMICS__);
#    endif
#  else
	printf("\tThe compilation supports atomic types and <stdatomic.h>.\n");
#  endif


#  ifdef __STDC_NO_COMPLEX__ /* __STDC_NO_COMPLEX__ - Complex types */
#    if __STDC_NO_COMPLEX__ == 1
	printf("\t\tThe compilation does NOT support complex types.\n");
#    else
	printf("\t\t__STDC_NO_COMPLEX__ = %d is an unexpected value.\n", __STDC_NO_COMPLEX__);
#    endif

#    ifdef __STDC_IEC_559_COMPLEX__
	printf("\t\tHowever, __STDC_IEC_559_COMPLEX__ is defined, and it should not be.\n");
#    endif
#  else
	printf("\tThe compilation supports complex types and <complex.h>.\n");
#  endif


#  ifdef __STDC_NO_THREADS__ /* __STDC_NO_THREADS__ - Multithreading */
	/*
	  If the macro constant __STDC_NO_THREADS__(C11) is defined by
	  the compiler, the header <threads.h> and all of the names
	  listed there are not provided.
	*/

#    if __STDC_NO_THREADS__ == 1
	printf("\t\tThe compilation does NOT support threads.\n");
#    else
	printf("\t\t__STDC_NO_THREADS__ = %d is an unexpected value.\n", __STDC_NO_THREADS__);
#    endif
#  else
	printf("\tThe compilation supports threads and <threads.h>.\n");
#  endif


#  ifdef __STDC_NO_VLA__ /* __STDC_NO_VLA__ - Variable length arrays */
#    if __STDC_NO_VLA__ == 1
	printf("\t\tThe compilation does NOT support variable length arrays (VLA).\n");
#    else
	printf("\t\t__STDC_NO_VLA__ = %d is an unexpected value.\n", __STDC_NO_VLA__);
#    endif
#  else
	printf("\tThe compilation supports variable length arrays.\n");
#  endif


#endif /* C or C++ */
}

int main(void)
{
	// eval the configuration, in case use the function in another implementation
	configuration();

	printf("\nREADY.\n");
	exit(EXIT_SUCCESS);
}


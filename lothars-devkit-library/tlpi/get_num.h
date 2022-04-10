/*
  taken from The Linux Programming Interface, Michael Kerrisk, 2010
*/

#ifndef GET_NUM_H
#define GET_NUM_H


#define GN_NONNEG    01 /* value must be >= 0 */
#define GN_GT_0      02 /* value must be > 0 */

/* by default integers are decimal */
#define GN_ANY_BASE  0100 /* can use any baes - like strtol(3) */
#define GN_BASE_8    0200 /* value is expressed in octal */
#define GN_BASE_16   0400 /* vlaue is expressed in hexadecimal */

long get_long(const char *arg, int flags, const char *name);
int get_int(const char *arg, int flags, const char *name);


#endif

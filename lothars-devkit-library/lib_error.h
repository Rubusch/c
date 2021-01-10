#ifndef DEVKIT_LIB_ERROR
#define DEVKIT_LIB_ERROR


// constants

#define MAXLINE 4096 /* max text line length */


// forwards

static void err_doit(int, const char *, va_list);

void err_dump(const char *, ...);
void err_msg(const char *, ...);
void err_quit(const char *, ...);
void err_ret(const char *, ...);
void err_sys(const char *, ...);


#endif /* DEVKIT_LIB_ERROR */

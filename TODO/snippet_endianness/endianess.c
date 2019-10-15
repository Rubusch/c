// endianess.c
/*
  endianness and portable code
  int is guaranteed to have 16 bit at least,
  long is guaranteed to have at least 32 bit
  
  x86 - is intel / little endian
  powerpc - is motorola / big endian
//*/


// not portable code, e.g.
int i = 4;
char c = *( char * )i;

// also not portable is
write(fd, &i, sizeof(i));

// a portable example might be
int j;
char buf[4];
ssize_t ret;
for (j = 0; j < 4; ++j) {
  buf[j] = (i >> (j * 8)) & 0xff;
}
if (0 > (ret = write(fd, buf, 4))) {
  fprintf(stderr, "%d; %d - write failed!", __FILE__, __LINE__);
}

// memcpy here is not portable!

// prifindex.c
/*
  prifindex <if>
  e.g.
  ./prifindex eth1
//*/

#include "../lib_socket/lib_ifi/ifi.h"


int main(int argc, char **argv)
{
  if(argc != 2){
    err_quit("usage: prifname <interface-name>");
  }
  
  printf("interface index = %d\n", _if_nametoindex(argv[1]));
  exit(0);
}

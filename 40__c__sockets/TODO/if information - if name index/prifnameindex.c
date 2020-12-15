// prifnameindex.c
/*
  print all the interface names and indexes
//*/

#include "../lib_socket/lib_ifi/ifi.h"


int main(int argc, char **argv)
{
  int num;
  char ifname[IFNAMSIZ];
  struct if_nameindex *ifptr=NULL, *save=NULL;
    
  for( save = ifptr = _if_nameindex()
        ; 0 < ifptr->if_index
        ; ++ifptr){

    printf("name = %s, index = %d\n", ifptr->if_name, ifptr->if_index);
    
    if(ifptr->if_index != (num = _if_nametoindex(ifptr->if_name))){
      err_quit("if_nametoindex returned %d, expected %d, for %s", num, ifptr->if_index, ifptr->if_name);
    }    

    _if_indextoname(ifptr->if_index, ifname);

    if(0 != strcmp(ifname, ifptr->if_name)){
      err_quit("if_indextoname returned %s, expected %s, for %d", ifname, ifptr->if_name, ifptr->if_index);
    }
  }
  
  num = if_nametoindex("fkjhkjhgjhgjhgdjhguyetiuyiuyhkjhkjdh");
  if(num != 0){
    err_quit("if_nametoindex returned %d for fkjh...", num);
  }

  num = if_nametoindex("");
  if(0 != num){
    err_quit("if_nametoindex returned %d for (null)", num);
  }
  
  if(NULL != if_indextoname(0, ifname)){
    err_quit("if_indextoname error for 0");
  }

  if(NULL != if_indextoname(888888, ifname)){
    err_quit("if_indextoname error for 888888");
  } 
 
  if_freenameindex(save);
  exit(0);
}

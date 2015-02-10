// select.c
/*
 * @author: Lothar Rubusch
 * @email: L.Rubusch@gmx.ch
 * @license: GPLv3
 *
 * int select(int nfds, fd_set* readfds, fd_set* writefds,
 *     fd_set* exceptfds, struct timeval* timeout);
 *
 * uses a "struct timeval timeout" (ms) which can be updated and has no sigmask
 *
 * int pSelect(int nfds, fd_set* readfds, fd_set* writefds,
 *     fd_set* exceptfds, const struct timespec* timeout,
 *     const sigset_t* sigmask);
 *
 * uses a "struct timespec timeout" (ns) which cannot be updated, furthermore it
 * has a sigmask
 *
 * The select functions allow a program to monitor multiple file descriptors
 * ("set" of handles), waiting until one or more of the file descriptors become
 * "ready" for some class of I/O operation (e.g. input possible). A file
 * descriptor is considered ready if it is possible to perform the corresponding
 * I/O operation (e.g. read()) without blocking.
 *
 * Macros:
 * void FD_CLR(int fd, fd_set* set)     - remove fd from set
 * int FD_ISSET(int fd, fd_set* set)    - fd is part of a set?
 * void FD_SET(int fd, fd_set* set)     - add fd to a set
 * void FD_ZERO(fd_set* set)            - clear set
 *
//*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>


struct list_entry{
  int hSocket;
  struct list_entry *next;
};


struct list_type{
  struct list_entry *data;
  unsigned count;
};


#define BUF_SIZ 4096
#define PORT 7777


void init_list(struct list_type* list)
{
  list->data = NULL;
  list->count = 0;
}


void quit(const char* text)
{
  perror(text);
  exit(1);
}


int add_client(struct list_type* list, int hSocket)
{
  struct list_entry *entry;
  if( (entry = malloc(sizeof(*entry))) == NULL) quit("malloc() failed");

  entry->hSocket = hSocket;
  entry->next = list->data;
  list->data = entry;
  list->count++;

  return 0;
}


int remove_client(struct list_type* list, int hSocket)
{
  if(!list->count) return 1;

  struct list_entry *pElement, *pElementBefore = NULL;
  for(pElement = list->data; pElement; pElement = pElement->next){
    if(pElement->hSocket == hSocket) break;
    pElementBefore = pElement;
  }

  if(!pElement) return 1;
  if(pElementBefore) pElementBefore->next = pElement->next;
  else list->data = pElement->next;

  free(pElement);
  list->count--;

  return 0;
}


int fill_set(fd_set* set, struct list_type* list)
{
  int max = 0;
  struct list_entry* pElement;

  for(pElement = list->data; pElement; pElement = pElement->next){

    if(pElement->hSocket > max) max = pElement->hSocket;

    FD_SET(pElement->hSocket, set);
  }

  return max;
}


int get_sender(fd_set* set)
{
  int idx = 0;

  while(!FD_ISSET(idx, set)) ++idx;

  return idx;
}


/*
  Special case: STDIN_FILENO needs to be writen on STDOUT_FILENO
//*/
int send_all(char* message, unsigned int message_len, struct list_type* list, int hSender)
{
  struct list_entry* pElement;

  for(pElement = list->data; pElement; pElement = pElement->next){

    if(pElement->hSocket == hSender) continue;

    if(pElement->hSocket == STDIN_FILENO) write(STDOUT_FILENO, message, message_len);
    else write(pElement->hSocket, message, message_len);
  }

  return 0;
}


int main_loop(int hSocket)
{
  int hClient, max, hSender, bytes;
  fd_set set;
  struct list_type list;
  char buf[BUF_SIZ];
  init_list(&list);
  add_client(&list, STDIN_FILENO);

  while(1){
    FD_ZERO(&set);
    max = fill_set(&set, &list);
    FD_SET(hSocket, &set);

    if(hSocket > max) max = hSocket;

    select(max + 1, &set, NULL, NULL, NULL); // S E L E C T ( ... )

    if(FD_ISSET(hSocket,  &set)){
      hClient = accept(hSocket, NULL, 0);
      add_client(&list, hClient);
    }else{
      hSender = get_sender(&set);
      bytes = read(hSender, buf, sizeof(buf));
      if(bytes == 0) remove_client(&list, hSender);
      else send_all(buf, bytes, &list, hSender);
    }
  }
}


int main(void)
{
  int hSocket;
  struct sockaddr_in addrServer;

  if(-1 == (hSocket = socket(PF_INET, SOCK_STREAM, 0))) quit("socket() failed");

  addrServer.sin_addr.s_addr = INADDR_ANY;
  addrServer.sin_family = AF_INET;
  addrServer.sin_port = htons(PORT);

  if(-1 == (bind(hSocket, (struct sockaddr*) &addrServer, sizeof(addrServer))))
    quit("bind failed");

  if(-1 == (listen(hSocket, 3)))
    quit("listen failed");

  return main_loop(hSocket);
}



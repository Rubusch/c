// comm_s.c - Socket Server (Linux)

/*
  socket - server
  compile:
  gcc -o comm_s comm_s.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>


extern size_t read( int sockfd, void* data, size_t data_len);
extern int atoi(const char* str);
extern int close(int sockfd);
extern int fork();
extern int wait(void*); // TODO: check ???


// macro for a more convenient output of IP addresses using printf()
#define NIPQUAD(addr) \
  ((unsigned char*) &addr)[0], \
  ((unsigned char*) &addr)[1], \
  ((unsigned char*) &addr)[2], \
  ((unsigned char*) &addr)[3]


#define BUFSIZE 1024


// globals
char global_buf[BUFSIZE + 1];


// signal handler to receive the SIGCHLD-signal to terminate child processes, 
// without they stay as zombies
void sighandler(int dummy)
{
  wait(NULL);
}


/*
  client functionality:
  - read available characters from socket into the buffer
  - search for EOL's, in case or if the buffer is full: print a message, 
      push the rest to the top and repeat
  - cancel, in case of an error or if the connection has quit
*/
void server(int hSocket, struct sockaddr_in* peer)
{
  int bytes_left, bytes_read;
  char *local_buf, *ptr_buf;

  ptr_buf = local_buf = global_buf; 
  bytes_left = BUFSIZE;
  while(1){
    if((bytes_read = read(hSocket, local_buf, bytes_left)) <= 0) 
      break;
   
    local_buf += bytes_read; 
    bytes_left -= bytes_read;
    
    while((ptr_buf < local_buf) && (*ptr_buf != '\n')) 
      ptr_buf++;
    
    while((ptr_buf < local_buf) || !bytes_left){
      *ptr_buf = 0;

      printf("message from %i.%i.%i.%i %i: %s\n",
	     NIPQUAD(peer->sin_addr.s_addr), ntohs(peer->sin_port), global_buf);

      if(ptr_buf < local_buf) 
	++ptr_buf;

      memmove(global_buf, ptr_buf, local_buf - ptr_buf);
      bytes_read = ptr_buf - global_buf; // Number of completed characters
      local_buf -= bytes_read; 
      bytes_left += bytes_read;
      ptr_buf = global_buf;
      
      while((ptr_buf < local_buf) && (*ptr_buf != '\n')) 
	++ptr_buf;
    }
  }
  
  if(bytes_read < 0) 
    perror("read");

  else if(local_buf > global_buf) { // print the rest
    *local_buf = 0; 
    printf("message from %i.%i.%i.%i %i: %s\n",
	   NIPQUAD(peer->sin_addr.s_addr), ntohs(peer->sin_port), global_buf);
  }
}


/*
  main program
  - completed arguments
  - open socket and wait until the connection is established
  - start proper process for each connection
*/
int main(int argc, char** argv)
{
  int s;
  struct sockaddr_in myaddr;
  int optval;

  if(argc != 2){
    fprintf(stderr, "Usage: %s <port>\n", argv[0]); 
    exit(1);
  }

  if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("socket");
    exit(1);
  }

  // socket option SO_REUSEADDR: allow bind() even if old 
  // protocol instances are still using the address
  optval = 1;
  if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))){
    perror("setsockopt"); 
    exit(1);
  }

  memset(&myaddr, 0, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_port = htons(atoi(argv[1]));
  myaddr.sin_addr.s_addr = INADDR_ANY;

  if(bind(s, (struct sockaddr*) &myaddr, sizeof(myaddr))){
    perror("bind");
    exit(1);
  }

  // SOMAXCONN = max number of connections
  if(listen(s, SOMAXCONN)){
    perror("listen");
    exit(1);
  }

  // install signal handler for SIGCHILD
  if(signal(SIGCHLD, sighandler) == SIG_ERR){
    perror("signal");
    exit(1);
  }

  while(1){
    int new_s;
    struct sockaddr_in claddr;
    socklen_t claddrlen;
    claddrlen = (socklen_t) sizeof(claddr);

    if((new_s = accept(s, (struct sockaddr*) &claddr, &claddrlen)) < 0){
      perror("accept");
      continue;
    }

    int rc=0;
    if(-1 == (rc = fork())){
    }else if(rc != 0){
      // parent process

      // new socket is used only by the child
      close(new_s);

    }else{
      // child process
     
      // old socket is used only by the parent
      close(s);
      printf("connection from %d.%d.%d.%d %d\n",
	     NIPQUAD(claddr.sin_addr.s_addr), ntohs(claddr.sin_port));
      server(new_s, &claddr);
      printf("connection from %d.%d.%d.%d %d closed\n",
	     NIPQUAD(claddr.sin_addr.s_addr), ntohs(claddr.sin_port));
      exit(0);
    } // missing: fork() failed! -1
  }    
}

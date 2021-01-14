// scratchy.c
/*
  client
//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "../unixsocket.h"

#define ME "SCRATCHY:"
#define MESSAGE "Scratchymessage"


int main(int argc, char **argv)
{
  int sd_client = -1;
  char buf[BUF_SIZE];
  char message[] = MESSAGE;
  memset(buf, '\0', BUF_SIZE);
  struct sockaddr_un adr_server;

  do {
    // client socket
    fprintf(stderr, "%s set up client socket\n", ME);
    if (0 > (sd_client = socket(AF_UNIX, SOCK_STREAM, 0))) {
      perror("socket failed");
      break;
    }

    // set up server address
    fprintf(stderr, "%s set up server address\n", ME);
    memset(&adr_server, 0, sizeof(adr_server));
    adr_server.sun_family = AF_UNIX;
    strncpy(adr_server.sun_path, SERVER_PATH, 1 + strlen(SERVER_PATH));

    // connect to server
    fprintf(stderr, "%s connect to the server\n", ME);
    if (0 > connect(sd_client, ( struct sockaddr * )&adr_server, sizeof(adr_server))) {
      perror("connect failed");
      break;
    }

    // send message
    fprintf(stderr, "%s send \"%s\"\n", ME, message);
    strncpy(buf, message, strlen(message));
    if (0 > send(sd_client, buf, sizeof(buf), 0)) {
      perror("send failed");
      break;
    }

    sleep(1);

    // receive message
    memset(buf, '\0', BUF_SIZE);
    register int bytes = 0;
    if (0 > (bytes = recv(sd_client, buf, BUF_SIZE - bytes, 0))) {
      perror("recv failed");
      break;

    } else {
      buf[bytes] = '\0';
      fprintf(stderr, "%s received \"%s\"\n", ME, buf);
      break;
    }

  } while (0);

  // in case something crashed - clean up
  if (-1 != sd_client)
    close(sd_client);

  fprintf(stderr, "%s done!\n", ME);
  exit(EXIT_SUCCESS);
}

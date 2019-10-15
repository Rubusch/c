// client.c
/*
  demonstrates the creation and use of a unix socket - AF_UNIX!!!
//*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "../unixsocket.h"

char *strings[NUMBER_OF_STRINGS] = {
    "this is the first string form the client.",
    "this is the second string from the client.",
    "this is the third string from the client."};


int main(int argc, char **argv)
{
  // socket
  register int sd_client = 0;
  if (0 > (sd_client = socket(AF_UNIX, SOCK_STREAM, 0))) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // address - Unix (cast to sockaddr possible!)
  struct sockaddr_un adr_server;
  adr_server.sun_family = AF_UNIX;
  strncpy(adr_server.sun_path, SERVER_PATH, 1 + strlen(SERVER_PATH));

  // connect
  //  register int len_adr_server = sizeof(adr_server.sun_family) +
  //  strlen(adr_server.sun_path);
  if (0 > (connect(sd_client, ( struct sockaddr * )&adr_server,
                   SUN_LEN(&adr_server)))) {
    perror("connect failed");
    exit(EXIT_FAILURE);
  }


  puts("connected");

  register int bytes = 0;
  register int idx = 0;
  char strReceived[RECEIVED_SIZE];
  do {
    // init
    memset(strReceived, '\0', RECEIVED_SIZE);
    bytes = 0;

    // send
    printf("send \"%s\"\n", strings[idx]);
    if (0 > send(sd_client, strings[idx], strlen(strings[idx]), 0)) {
      perror("send failed");
      break;

    } else {
      // next string
      ++idx;

      // receive
      if ((0 >= (bytes = recv(sd_client, strReceived, RECEIVED_SIZE, 0))) &&
          (bytes < RECEIVED_SIZE)) {
        perror("recv failed");
      } else {

        strReceived[bytes] = '\0';
        printf("received: \"%s\"\n", strReceived);
      }
    }
  } while (idx < NUMBER_OF_STRINGS);

  // close the socket
  puts("READY.");
  close(sd_client);
  exit(EXIT_SUCCESS);
}

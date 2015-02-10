// itchy.c
/*
  message queue - receiver

  the receiving client in a messagequeue
  gcc needs "-D _SVID_SOURCE=1 -D _XOPEN_SOURCE=600" to compile this
//*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h> // ftok(), msgget()
#include <sys/ipc.h> // ftok(), msgget()
#include <sys/msg.h> // msgget()

#include "../messagequeue.h"

#define ME "ITCHY:"


int main(int argc, char** argv)
{
  fprintf(stderr, "%s started\n", ME);

  // message queue key - just any file(name) and character serve
  fprintf(stderr, "%s set up message queue key\n", ME);
  key_t mq_key=0;
  if(0 > (mq_key = ftok("../messagequeue.h", 'B'))){
    perror("ftok failed");
    exit(EXIT_FAILURE);
  }

  // message queue id - connect to the queue, 0666 are the privileges
  fprintf(stderr, "%s set up message queue ID\n", ME);
  int mq_id=0;
  if(0 > (mq_id = msgget(mq_key, 0666))){
    perror("msgget failed");
    exit(EXIT_FAILURE);
  }

  // receive - reads the first message on the queue, and blocks until it has a message
  msgbuf mq_message;

  if(0 > msgrcv(mq_id, &mq_message, BUF_SIZE, 1, 0)){
    perror("msgrcv failed");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "%s received \"%s\"\n", ME, mq_message.buf);

  fprintf(stderr, "%s done!\n", ME);
  exit(EXIT_SUCCESS);
}

# Message Queue Demo

## Usage

```
$ ./mq_sample.elf
build messagequeue

your options are:
exit = 0 or CTRL+c
msgsnd = 1
msgrcv = 2

enter your choice (0/1/2):
1
enter a one line message
asdf
vailable msgsnd flags:
IPC_NOWAIT = 00004000
enter msgflg (y/n):
 y
summary
msgop: calling msgsnd(1, msg_ptr, 4, 04000)
msg_ptr->mtype = 34
msg_ptr->mtext = "asdf"
msgop: msgsnd returned 0

your options are:
exit = 0 or CTRL+c
msgsnd = 1
msgrcv = 2

enter your choice (0/1/2):
 2
meaningful msgrcv flags are:
1. MSG_NOERROR = 00010000
2. IPC_NOWAIT = 00004000

enter msg_flag (1 or 2):
 2
msgop: calling msgrcv(1, msg_ptr, 24, 34, 0);
msgop: msgrcv returned 24
msg_ptr->mtype = 34
msg_ptr->mtext = "asdf"

your options are:
exit = 0 or CTRL+c
msgsnd = 1
msgrcv = 2

enter your choice (0/1/2):
 0
READY.
```

NB: The message queue will be persistent (in temp file system) and entries are not removed automatically. Starting the demo over and over, thus will yield artifacts of the former runs.  

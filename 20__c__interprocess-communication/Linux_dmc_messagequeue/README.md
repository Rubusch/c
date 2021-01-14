# Messagequeue

## Build

```
$ cd receiver
$ make
$ cd -
$ cd sender
$ make
```


## Usage

terminal 1 (sender):  

```
$ ./message_send.exe 

msgget: calling msgget(0x42210115l, 01666)
msgget: msgget succeeded: mq_id = 0
msgget: msgget succeeded: mq_id = 0
msgget: msgget succeeded: mq_id = 0
message: "can you dig it?" sent
READY.
```

terminal 2 (receiver):  

```
$ ./message_rec.exe
can you dig it?
READY.
```

NB: The sender may exit before the receiver starts, the message queue persists.  

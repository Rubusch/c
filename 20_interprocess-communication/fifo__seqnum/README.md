## Fifo (pipe)

### Usage

client and server - start server  

```
$ cd ./fifoserver
$ make clean && make
$ ./fifoserver.elf &
```

build and start client  

```
$ cd ../fifoclient
$ make clean && make
$ ./fifoclient.elf
  client: seqnum 0
  server: seqnum 1
  READY.

$ ./fifoclient.elf
  server: seqnum 2
  client: seqnum 1
  READY.

$ ./fifoclient.elf 5
  server: seqnum 8
  client: seqnum 7
  READY.
```


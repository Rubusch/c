# Unix Socket Demo

A simple client-server application that uses stream sockets in the
UNIX domain. The client program connects to the server, and uses the
connection to transfer data from its standard input to the server.  

The server program accepts client connections, and transfers all data
sent on the connection by the client to standard output. The server is
a simple example of an iterative server - a server that handles one
client at a time before proceeding to the next client.  

Based on The Linux Programming Interface, Michael Kerrisk, 2010,
p. 1169  


## Usage

In the server folder  

```
$ ./server.exe &
  [1] 2534
```


In the client folder  

```
$ ./client.elf hello world
  server received 5 bytes from /tmp/unixsocket.2537
  response 1: HELLO
  server received 5 bytes from /tmp/unixsocket.2537
  response 2: WORLD
  client: done!

$ ./client.elf 'longer message'
  server received 14 bytes from /tmp/unixsocket.2549
  response 1: LONGER MESSAGE
  client: done!

$ kill %1
```

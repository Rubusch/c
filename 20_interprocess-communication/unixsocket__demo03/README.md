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
$ ./server.elf > b &
[1] 4501

$ ls -lF /tmp/unixdomain-transfer
srwxr-xr-x 1 pi pi 0 Aug 15 21:55 /tmp/unixdomain-transfer=
```


In the client folder  

```
$ echo "foobar" > a

$ ./client.elf < a
```

Now stop the server  

```
$ kill %1
[1]+  Terminated              ./server.elf > b  (wd: /usr/src/github__c/20_interprocess-communication/unixsocket__demo03/unixserver)

$ diff a ../unixserver/b
  <empty>
```


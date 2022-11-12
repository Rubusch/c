# Iterative Server for UDP ECHO Service

Servers for the _echo_ service. The _echo_ service operates on both
UDP and TCP port 7 (since this is a reserved port, the _echo_ server
must be run with **superuser** privileges).

Based on The Linux Programming Interface, Michael Kerrisk, 2010,
p. 1240 ff  


Client and server implement the echo server by struct addrinfo, a
queue for struct addrinfo instances. As an alternative struct
sockaddr_in would be a generalized but unqueued approach.  

NB: It is possible to open socket connections via `struct sockaddr`.

`struct sockaddr`  

This structure seerves as a template for all of the domain-specific
address structures. Each of these address structures begins with a
family field corresponding to the `sa_family` field of the `struct
sockaddr`. (The `sa_family_t` data type is an integer type specified
in SUSv3). [M. Kerrisk, p. 1154]  


`struct addrinfo`  

For convenience there is `getaddrinfo()` returning a queue of `struct
addrinfo` elements. This is what happenes here. It walks through the
devices to listen for connections (server) by checking the next
pointer of the `struct addrinfo` instance(s).  


## Usage

In the server folder  

```
$ sudo ./server.elf
  [1] 2832
```

In the client folder  

```
$ ./client.elf 127.0.0.1 hello world
    CLIENT - [5 bytes] 'hello'
    CLIENT - [5 bytes] 'world'
    CLIENT - done!
```

NB: Don't forget to kill the server which is running as a daemon in the background.  

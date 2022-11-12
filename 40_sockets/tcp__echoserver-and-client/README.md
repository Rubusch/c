# Concurrent Server for TCP ECHO Service

- The server becomes a daemon  

- Since the server creates a child process for each client connetion,
  we must ensure that zombies are reaped. We do this within a
  `SIGCHLD` handler.  

- The main body of the server consists of a `for` loop that accepts a
  client connection and then uses `fork()` to create a child process
  that invoke the `handle_request()` function to handle that
  client. In the meantime, the parent continues around the `for` loop
  to acceppt the next client
  connection.  

- In a real-world application, we would probably include some code in
  our server to place an upper limit on the number of child processes
  that the server could create, in order to prevent an attacker from
  attempting a remote fork bomb by using the service to create so many
  processes on the system that it becomes unusable. We could impose
  this limit by adding extra code in the server to count the number of
  children currently executing (this count would be incremented after
  a successful `fork()` and decremented as each child was reaped in
  the SIGCHLD handler). If the limit on the number of children were
  reached, we could then temporarily stop accepting connections (or
  alternatively, accept connections and then immediately close them).  

- After each `fork()`, the file descriptors for the listening and
  connected sockets are duplicated in the child. This means that the
  parent and the child could communicate with the client using the
  connected socket. However, only the child needs to perform such
  communication, and so the parent closes the file descriptor for the
  connected socket immediately after the `fork()`. If the parent did
  not do this, then the socket would never actually be closed;
  furthermore, the parent would eventually run out of file
  descriptors. Since the child doesn't accept new connections, it
  closes its duplicate of the file descripptor for the listening
  socket.  

- Each child process terminates after handling a single client.  

Based on The Linux Programming Interface, Michael Kerrisk, 2010,
p. 1243 ff  


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

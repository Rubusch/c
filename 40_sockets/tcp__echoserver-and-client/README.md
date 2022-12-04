# Concurrent Server for TCP ECHO Service

- The server becomes a daemon  

- Since the server creates a child process for each client connetion,
  we must ensure that zombies are reaped. We do this within a
  `SIGCHLD` handler.  

- The main body of the server consists of a `for` loop that accepts a
  client connection and then uses `fork()` to create a child process
  that invoke the `handle_request()` function to handle that
  client. In the meantime, the parent continues around the `for` loop
  to acceppt the next client connection.  

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
$ ./server.elf ; sudo tail -f /var/log/syslog | grep SERVER
    Dec  4 20:42:43 raspberrypi server.elf: SERVER: main() - listen()
    Dec  4 20:42:43 raspberrypi server.elf: SERVER: inet_passive_socket() - start
    Dec  4 20:42:43 raspberrypi server.elf: SERVER: inet_passive_socket() - port: 27978
    Dec  4 20:42:43 raspberrypi server.elf: SERVER: main() - accept(), waiting...
    Dec  4 20:42:49 raspberrypi server.elf: SERVER: main() - accept(), accepted!
    Dec  4 20:42:49 raspberrypi server.elf: SERVER: main() - fork()
    Dec  4 20:42:49 raspberrypi server.elf: SERVER: main() - child
    Dec  4 20:42:49 raspberrypi server.elf: SERVER: main() - child of accepted connection socket
    Dec  4 20:42:49 raspberrypi server.elf: SERVER: handle_request() -  start
    Dec  4 20:42:49 raspberrypi server.elf: SERVER: main() - child
    Dec  4 20:42:49 raspberrypi server.elf: SERVER: main() - child of accepted connection socket
    Dec  4 20:42:49 raspberrypi server.elf: SERVER: handle_request() -  start
    Dec  4 20:42:49 raspberrypi server.elf: SERVER: handle_request() - buf = 'Humpty Dumpty sat on a wall. Humpty Dumpty had a great fall. All the king's horses and all the king'd'
^C

$ sudo killall server.elf
```

In the client folder  

```
$ ./client.elf 127.0.0.1 < ./input.txt
    CLIENT: host = '127.0.0.1' port = '27978'
    CLIENT: inet_connect() - start
    CLIENT: inet_connect() - socket()
    CLIENT: inet_connect() - connect( 3, 17322404, 16 )
    CLIENT: fork()
    parent: read() from STDIN
    child: read()
    parent: STDIN read something: buf = 'Humpty Dumpty sat on a wall. Humpty Dumpty had a great fall. All the king's horses and all the king's men couldn't put Humpty together again.��', nread = 141
    parent: write('Humpty Dumpty sat on a wall. Humpty Dumpty had a great fall. All the king's horses and all the king's men couldn't put Humpty together again.��')
    child: read something: buf = 'Humpty Dumpty sat on a wall. Humpty Dumpty had a great fall. All the king's horses and all the king'�0', nread = 100
    child: Humpty Dumpty sat on a wall. Humpty Dumpty had a great fall. All the king's horses and all the king'child: read()
    child: read something: buf = 's men couldn't put Humpty together again.y had a great fall. All the king's horses and all the king'�0', nread = 41
    child: s men couldn't put Humpty together again.child: read()
    parent: exiting
    child: read something: buf = 's men couldn't put Humpty together again.y had a great fall. All the king's horses and all the king'�0', nread = 0
    child: exiting, nread = '0'
```

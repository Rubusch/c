# UDP IPv6 Client-Server Example

Both the server and the client include a common header file. This file includes various other header files, and defines the TCP port number to be used by the application.  

Based on The Linux Programming Interface, Michael Kerrisk, 2010,
p. 1219 ff  


## Server

The server program shown performs the following steps:  

- Initialize the server's sequence number either to 0 or to the value supplied in the optional command-line argument.  

- Ignore the ``SIGPIPE`` signal. This prevents the server from receiving the ``SIGPIPE`` signal if it tries to write to a socket whose peer has been closed; instead, the ``write()`` fails with the error ``EPIPE``.  

- Call ``getaddrinfo()`` to obtain a set of socket address structures for a TCP socket that uses the port number ``PORT_NUM``. (Instead of using a hard-coded port number, we would more typically use a service name). We specify the ``AI_PASSIVE`` flag so that the resulting socket will be bound to the wildcard address. As a result, if the server is run on a multihomed host (e.g. as the great feature used in SCTP protocol), it can accept connection requests sent to any of the host's network addresses.  

- Enter a loop that iterates through the socket address structures returned by the previous step. The loop terminates when the program finds an address structure that can be used to successfully create and bind a socket.  

- Set the ``SO_REUSEADDR`` option for the socket created in the previous step. NB: a TCP server sould usually set this option on its listineing socket.  

- Mark the socket as a listening socket.  

- Commence an infinite ``for`` loop that services clients iteratively. Each client's request is serviced before the next client's request is accepted. For each client, the server performs the following steps:  

- - Accept a new connection. The server passes non-NULL pointers for the second and third argument to ``accept()``, in order to obtain the address of the client. The server displays the client's address (IP address plus port number) on standard output.  

- - Read the client's message, which consists of a newline-terminated string specifying how many sequene numbers the client wants. The server converts this string to an integer and stores it in the variable ``reqlen``.

- - Send the current value of the sequence number (``seqnum``) back to the client, encoding it as a newline-terminated string. The client can assume that it has been allocated all of the sequence numbers in the range ``seqnum`` to ``seqnum + reqlen - 1``.

- - Update the value of the server's sequence number by adding ``reqlen`` to ``seqnum``.


## Client

The client accepts two arguments. The first argument, which is the name of the host on which the server is running, is mandatory. The optional second argument is the length of the sequence desired by the client. The default length is 1. The client performs the following steps:  

- Call ``getaddrinfo()`` to obtain a set of socket address structures suitable for connecting to a TCP server bound to the specified host. For the port number, the client specifies ``PORT_SERVER``.  

- Enter a loop that iterates through the socket address structures returned by the previous step, until the client finds one that can be used to successfuly create and connect a socket to the server. Since the client finds one that can be used to successfully create and connect a socket to the server. Since the client has not bound its socket, the ``connect()`` call causes the kernel to assign an ephemeral port to the socket.  

- Send an integer specifying the length of the client's desired sequence. This integer is sent as a newline-terminated string.  

- Read the sequence number sent back by the server (qhich is likewise a newline-terminated stirng) and print it on standard output.  


## Usage

In the server folder  

```
$ ./server.exe &
  [1] 2832
```


In the client folder  

```
$ ./client.elf localhost
  connection from (localhost, 57414)
  sequence number: 0

  done!

$ ./client.elf localhost 10
  connection from (localhost, 57420)
  sequence number: 1

  done!

$ ./client.elf localhost
  connection from (localhost, 57424)
  sequence number: 11

  done!
```

Use telnet for debugging this/such application, e.g. as follows  

```
$ telnet localhost 50000
  Trying ::1...
  Trying 127.0.0.1...
  connection from (localhost, 57610)
  Connected to localhost.
  Escape character is '^]'.
  1
  12
  Connection closed by foreign host.
```


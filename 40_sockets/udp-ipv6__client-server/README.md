# UDP IPv6 Client-Server Example

The clinet and server both emply the header udp-ip6.h  

The server uses the inet_ntop() function to convert the host address of the client (obtained via the recvfrom() call) to printable form.  

The client program shown contains two notable modifications (compared to UNIX domain version in TLPI, p 1173). The first difference is that the clinet interprets its initial command-line argument as the IPv6 address of the server. (The remaining command-line arguments are passed as separate datagrams to the server). The client converts the server address to binary from using inet_pton(). The other difference is that the client doesn't bind its socket to an address. As noted if an Internet Domain socket is not bound to an address, the kernel binds the socket to an ephemeral port on the host system. We can observe this in the following shell session log, where we run the server and the client on the same host.  

Based on The Linux Programming Interface, Michael Kerrisk, 2010,
p. 1207 ff  


## Usage

In the server folder  

```
$ ./server.exe &
  [1] 14219
```


In the client folder  

```
$ ./client.elf ::1 "ciao bella!"
  server received 10 bytes from (::1, 35828)
  response 1: CIAO BELLA
  done!
```

From the above output, we see that the server's recvfrom() call was able to obtain the address of the client's socket, including the ephemeral port number, despite the fact that the client did not do a bind().  

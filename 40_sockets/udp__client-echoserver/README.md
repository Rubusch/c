# Iterative Server for UDP ECHO Service

Servers for the _echo_ service. The _echo_ service operates on both UDP and TCP port 7 (since this is a reserved port, the _echo_ server must be run with **superuser** privileges).

Based on The Linux Programming Interface, Michael Kerrisk, 2010,
p. 1240 ff  


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


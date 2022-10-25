# Iterative Server for UDP ECHO Service

Servers for the _echo_ service. The _echo_ service operates on both
UDP and TCP port 7 (since this is a reserved port, the _echo_ server
must be run with **superuser** privileges).

Based on The Linux Programming Interface, Michael Kerrisk, 2010,
p. 1240 ff  


## Usage

In the server folder  

```
$ sudo ./server.elf &
  [1] 2832
```


In the client folder  

```
$ ./client.elf localhost hello world!
TODO
```


## socket implementations

The origins of those code snippets reach back into 2007 when I was working in telecommunications, learning most of the stuff from W. Richard Stevens' unix books.  

Most applications are demos. They can be build by executing ``make`` in the specific directory. They should explain themselves either by detailed comments in the source code, or when executing by a displayed help.  


#### ioctl

Some examples for reading out interface information through ``ioctl()`` calls.  



#### tcp server types

Open a shell, build and start the specific server with a provided port number, or execute w/o argument for help  

```
$ make
$ ./server.exe 270976
```

Open another shell, build and start the specific client, provide the server IP and port number  

```
$ make
$ ./client.exe 127.0.0.1 270976
```


#### References

Many codes are based on material taken from the classics on unix network programming   

* UNIX Network Programming. Volume 1, W. R. Stevens
* UNIX Network Programming. Volume 2, W. R. Stevens
* TCP/IP Illustrated. Volume 1: The Protocols, W. R. Stevens
* TCP/IP Illustrated. Volume 2: The Implementation, W. R. Stevens
* TCP/IP Illustrated. Volume 3: TCP for Transactions, HTTP, NNTP, and the UNIX Domain Protocols, W. R. Stevens

# TODO

some notes, missing examples, etc.

## fix deprecated signal handler usages
./20__c__interprocess-communication/sharedmemory__shm_attach_detach__TODO/shm_attachdetach.c

$ grep "signal(" -HIrn . | grep -v "DEPRECATED" | awk -F: '{ print $1 }' |sort|uniq

./40__c__sockets/tcp-server-types__1-tcp-concurrent-server-one-child-per-client/server.c
./40__c__sockets/tcp-server-types__2a-tcp-preforked-no-locking-around-accept/server.c
./40__c__sockets/tcp-server-types__2b-tcp-preforked-no-locking-around-accept-using-select/server.c
./40__c__sockets/tcp-server-types__3-tcp-preforked-server-file-locking-around-accept/server.c
./40__c__sockets/tcp-server-types__4-tcp-preforked-server-thread-locking-around-accept/server.c
./40__c__sockets/tcp-server-types__5-tcp-preforked-server-descriptor-passing/server.c
./40__c__sockets/tcp-server-types__6-tcp-concurrent-server-on-thread-per-client/server.c
./40__c__sockets/tcp-server-types__7-tcp-prethreaded-server-per-thread-calls-accept/server.c
./40__c__sockets/tcp-server-types__8-tcp-prethreaded-server-main-thread-calls-accept/server.c
./40__c__sockets/tcp-udp-server__echoserver/server.c
./40__c__sockets/udp-client__reliable/client.c
./lothars-devkit-library/lib_pthread.c
./lothars-devkit-library/lib_pthread.h
./lothars-devkit-library/lib_pthread__test.c
./lothars-devkit-library/lib_unix.c
./lothars-devkit-library/lib_unix.h
./lothars-devkit-library/lib_unix__test.c
./lothars-devkit-library/snippet_socket.c

## integrate signalhandler example



# udp-forwarder
A UDP forwarder that adds one byte at the end of each datagram to bypass filters.
It does not add an overhead like other possibilities to bypass filters and firewalls (except for the byte).

Usage
-----
- Create a file named forwarder.h with your config (you will need two config files for the server and the client) e.g:
  ```c
  #define SOURCE_PORT 1194
  #define DESTINATION_HOST "127.0.0.1"
  #define DESTINATION_PORT 1195
  #define MODE CLIENT|SERVER
  ```
  
- Than complie the application with gcc:
    ```
    $ gcc forwarder.c -Wall -o forwarder
    ```

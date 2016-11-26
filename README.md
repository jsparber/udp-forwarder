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
Description
----------
Some ISP are blocking and filtring network traffic. Normaly you don't expirence inconivince. There should be any restriction on a internet connection, but some ISP go a step forder they not only block torrent and websites with illigale content, also important protocols like webrtc and even the standart ports for e-mail(in mycase STARTSSL). When I can't connect to my e-mail account, that is the break line for me. So I tried using a self hosted vpn. It didn't work, nieder udp nor tcp traffic on diffrent open ports (there arn't that many, maby 15 ports). So, they block explicit packages throw some sort of deep packet inspection. I searched on the internet for some solution to hide openvpn traffic, but i couln't find anything to encrypt or mask udp traffic. There are some solution for tcp traffic like stunnel but nothing for udp. So I started to write my own simple app to hide my udp traffic. At the begining I used a quite simple meathot to obfuscate my traffic by adding one byte to the end of each datagram. Yay, the network filter isn't that inteligent, I recived some e-mails. I didn't liked the extra byte my app is adding to each datagram, what ever it works ;).
Lateron I change the algorithm to one a litte more complicateted to save one byte overhead. Datagrams are nothing diffrent than a sequence of chars. The tunnel rotats every char by one, so the datagram doesn't look like openvpn traffic, but it still is. Than on the server I used the same app to decode the the hidden content and it forwards the decoded traffic to the actual openvpn server. Some times it's realy easy to cercumctens nework restriction, some times les easy but there will be always a possibility. And the only people witch are suffering of network restriction are the people they don't harm enyone or are everge user. 

/*
   A UDP forwarder that adds one byte at the end of each datagram to bypass filters
   It does not add an overhead like other possibilities to bypass filters and firewalls (except for the byte)
   */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
//create this file with destination, source and mode defined
#include "forwarder.h"

#define BUF_LENGTH 212992
#define SERVER 0
#define CLIENT 1
void die(char *);
void wait();
char changeChar(char, int);

void die(char *s)
{
  perror(s);
  exit(1);
}

int main(void)
{
  pid_t childPID = -1;
  struct sockaddr_in si_src, si_dest, si_res_src, si_res_dest;
  int recv_len, socket_src, socket_dest;
  socklen_t slen_src = sizeof(si_src),  slen_dest = sizeof(si_dest);
  char buf[BUF_LENGTH];
  int currentPort = 0;
  int mode = MODE;

  //################################################
  //Source socket
  //################################################

  //create source UDP socket
  if ((socket_src = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
    die("socket src");
  }

  // zero out the structure
  memset((char *) &si_src, 0, sizeof(si_src));

  si_src.sin_family = AF_INET;
  si_src.sin_port = htons(SOURCE_PORT);
  si_src.sin_addr.s_addr = htonl(INADDR_ANY);

  //bind socket to si_src
  if(bind(socket_src, (struct sockaddr*) &si_src, sizeof(si_src)) == -1)
  {
    die("bind src");
  }

  //################################################
  //Destination socket
  //################################################

  //create destination UDP socket
  if ((socket_dest = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
    die("socket dest");
  }

  // zero out the structure
  memset((char *) &si_dest, 0, sizeof(si_dest));

  si_dest.sin_family = AF_INET;
  si_dest.sin_port = htons(DESTINATION_PORT);
  si_dest.sin_addr.s_addr = htonl(INADDR_ANY);

  if (inet_aton(DESTINATION_HOST , &si_dest.sin_addr) == 0) 
  {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  //keep listening for data
  while(1)
  {
    //Listening on clients datagram and forward it to the destination server
    if (childPID != 0) {
      if ((recv_len = recvfrom(socket_src, buf, BUF_LENGTH, 0, (struct sockaddr *) &si_res_src, &slen_src)) == -1)
      {
        die("recvfrom()");
      }

      //Add byte when the forwarder is in client mode else remove it
      int i;
      for (i = 0; i < recv_len; i++) {
        if (mode == CLIENT)
          buf[i] = changeChar(buf[i], 1);
        else
          buf[i] = changeChar(buf[i], -1);
      }

      //send datagram to the destination server
      if (sendto(socket_dest, buf, recv_len, 0, (struct sockaddr*) &si_dest, slen_src) == -1)
      {
        die("sendto()");
      }

      //Kill child process when there is a new client with new port
      if (childPID != -1 && currentPort != si_res_src.sin_port) {
        printf("New Client on port: %d", si_res_src.sin_port);
        kill(childPID, SIGKILL);
        wait(NULL);
        childPID = -1;
        currentPort = si_res_src.sin_port;
      }

      //if there is no child create one
      if (childPID < 0)
        childPID = fork();
    }

    //Forward datagrams form the destination server to the client
    if (childPID == 0) {
      if ((recv_len = recvfrom(socket_dest, buf, BUF_LENGTH, 0, (struct sockaddr *) &si_res_dest, &slen_dest)) == -1)
      {
        die("recvfrom()");
      }

      //Add byte when the forwarder is in server mode else remove it
      //printf("#############################################################\n");
      //printf("Got this msg form vpn server: %s\n", buf);
      int i;
      for (i = 0; i < recv_len; i++) {
        if (mode == SERVER)
          buf[i] = changeChar(buf[i], 1);
        else
          buf[i] = changeChar(buf[i], -1);
      }

      //printf("#############################################################\n");
      //printf("Send this msg to client: %s\n", buf);

      //send datagram to client
      if (sendto(socket_src, buf, recv_len, 0, (struct sockaddr*) &si_res_src, slen_dest) == -1)
      {
        die("sendto()");
      }
    }
  }
  close(socket_src);
  close(socket_dest);
  return 0;
}

char changeChar(char el, int inc) {
  return el + inc;
}


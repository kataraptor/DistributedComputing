/*
 * datagram_client2.c
 * COMP 3010 Distributed Computing
 * (C) Computer Science, University of Manitoba
 *
 * Create a datagram client (with basic error checking)
 * This version replaces gethostbyname with getaddrinfo
 *
 * to compile: gcc datagram_client2.c -o dgram_client
 */

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
typedef unsigned int uint;   // to keep the compiler happy


int main(int argc, char *argv[]) {
  int sfd;                                 // socket file descriptor
  int result;                              // result of function calls
  struct sockaddr_in from;                 // received-from socket address
  uint fromlen = sizeof(struct sockaddr);  // length of incoming address
  char buffer[256];                        // buffer for fgets

  // check cmdline args
  if (argc != 3) {
    printf("Usage: %s <server> <port>\n", argv[0]);
    exit(0);
  }

  // create a socket
  //
  // Arguments:
  //   Domain (AF_INET): IPv4 Internet protocols
  //   Type (SOCK_DGRAM): connectionless
  //   Protocol: (use 0 for IP)
  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1) {
    perror("Client couldn't open a socket");
    exit (1);
  }

  /* Use getaddrinfo to get the server's address information given the
     host/port entered on the command line. The result is returned in a
     linked list of addrinfo structs, shown below:

       int getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints,
                       struct addrinfo **res);

           struct addrinfo {
               int              ai_flags;
               int              ai_family;
               int              ai_socktype;
               int              ai_protocol;
               size_t           ai_addrlen;
               struct sockaddr *ai_addr;
               char            *ai_canonname;
               struct addrinfo *ai_next;
           };
  */

  struct addrinfo *hostaddr;
  result = getaddrinfo(argv[1], argv[2], NULL, &hostaddr);
  if(result != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(sfd));
    exit(EXIT_FAILURE);
  }

  // // send a datagram to the server and wait for a response
  // printf("Please enter your message: ");
  // bzero(buffer, 256);
  // fgets(buffer, 255, stdin);
  // result = sendto(sfd, buffer, strlen(buffer), 0, hostaddr->ai_addr, hostaddr->ai_addrlen);
  // if (result == -1) {
  //   perror("Client sendto failed");
  //   exit (1);
  // }

  //RNG TO GET THE NUMBER WE KEEP SENDING TO
  srand((unsigned) 7712173);

  int keepSending = (((rand()%21)+10) % 21);
  printf("\nkeep sending: %d\n", keepSending);

  int errors = 0;
  int i;
  int lineNum;
  int sleepTime;
  for(i = 0; i < keepSending; i++)
  {
    lineNum = rand() % 4001;
    printf("\nlinenum: %d\n", lineNum);
    sleepTime = (((rand()%6)+2) % 6);
    printf("\nsleepTime: %d\n", sleepTime);

    //convert to string
    sprintf(buffer, "%d", lineNum);
    printf("%s", buffer);

      result = sendto(sfd, buffer, strlen(buffer), 0, hostaddr->ai_addr, hostaddr->ai_addrlen);
      if (result == -1) {
        perror("Client sendto failed");
        exit (1);
      }

     if(recvfrom(sfd, buffer, 256, 0, (struct sockaddr *)&from, &fromlen))
     {
       errors ++;
     }

     printf("we got here")

    //sleep(sleepTime);
    //printf("hung Here")
  }

  //send last message to server:
  result = sendto(sfd, "-1", strlen(buffer), 0, hostaddr->ai_addr, hostaddr->ai_addrlen);
  if (result == -1) {
    perror("Client sendto failed");
    exit (1);
  }

  printf("\n\nALL DONE:\nNumber of valid req: %d\nNumber of invalid req: %d", (keepSending - errors), errors);

  // close the socket and exit
  close(sfd);
  exit(0);
} // main

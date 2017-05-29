/*
 * datagram_server2.c
 * COMP 3010 Distributed Computing
 * (C) Computer Science, University of Manitoba
 * 
 * Create a server that receives connectionless messages (with basic error checking)
 * This version replaces gethostbyname with getaddrinfo
 * 
 * to compile: gcc datagram_server2.c -o dgram_server
 */
#define BUFFSIZE 256
#define PORT "3010"
#define DIRECTORYSERVER "goose.cs.umanitoba.ca"


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
typedef unsigned int uint;

typedef struct file_info{
 char name[80];
 int num_lines;
} FILE_INFO;

int main(int argc, char *argv[]) {
  int sfd;                                 // socket file descriptor
  uint result;                             // return result from functions
  struct addrinfo hints;                   // specify criteria for getaddrinfo
  struct sockaddr_in from;                 // received-from socket address
  uint fromlen = sizeof(struct sockaddr);  // length of incoming address
  char buffer[256];                        // buffer for I/O

  // check cmdline args
  if (argc < 2) {
    printf("Usage: %s <port> (use the ports luke) actually tho put in one of the three they gave you\n", argv[0]);
    exit(1);
  }
  
  printf("butts %s", "test");

  /* create a socket
   *
   * Arguments:
   *   Domain (AF_INET): IPv4 Internet protocols
   *   Type (SOCK_DGRAM): connectionless
   *   Protocol: (use 0 for IP)  
   */
  sfd = socket(AF_INET, SOCK_DGRAM, 0); 
  if (sfd == -1) {
    perror("Server couldn't open a socket");
    exit (1);
  }

  /* Use getaddrinfo to get the server's address information given the
     port entered on the command line. The result is returned in a
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
  //TRYING TO TOUCH THE DIRECTORY SERVER
  printf("We are here");
  exit(1);
  struct addrinfo *texthostaddr;
  result = getaddrinfo(DIRECTORYSERVER, PORT, NULL, &texthostaddr);
  if(result != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(sfd));
    exit(EXIT_FAILURE);
  }
  ///////copied from client
  
  //sending and getting a message
  result = sendto(sfd, "please send me back stuff\n", 35, 0, texthostaddr->ai_addr, texthostaddr->ai_addrlen);
  if (result == -1) {
    perror("Client sendto failed");
    exit (1);
  }
  
  printf("hello? is there anybody out there");
  
  result = recvfrom(sfd, buffer, 256, 0, (struct sockaddr *)&from, &fromlen);
  if (result == -1) {
    perror("Client recvfrom failed");
    exit (1);
  }
  printf("Client: msg received was: %s\n", buffer);
  ////////
  
  
  
  //LETTING STUFF TALK TO YOU
  // set up a 'hints' struct to specify what kind of address we want
  bzero((void *)&hints, sizeof(hints));
  hints.ai_family = AF_INET;        // IPV4 address
  hints.ai_socktype = SOCK_DGRAM;   // datagram socket
  hints.ai_flags = AI_PASSIVE;      // suitable for accepting connections
 
  struct addrinfo *hostaddr;  
  result = getaddrinfo(NULL, argv[1], &hints, &hostaddr);
  if(result != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(sfd));
    exit(EXIT_FAILURE);
  }
  
  // use bind to assign a name to the socket
  result = bind(sfd, hostaddr->ai_addr, hostaddr->ai_addrlen);
  if (result == -1) {
    perror("Couldn't bind to server socket");
    exit (EXIT_FAILURE);
  }
  printf("Datagram server starting on port %s.\n", argv[1]);
  
  // wait for a message
  result = recvfrom(sfd, buffer, 256, 0, (struct sockaddr *)&from, &fromlen);
  if (result == -1) {
    perror("Server recvfrom failed");
    exit (EXIT_FAILURE);
  }
  printf("Server: received the datagram: ");
  buffer[result]= '\0'; // null terminate the string
  printf("%s.\n", buffer);
  result = sendto(sfd, "Got your msg\n", 14, 0, (struct sockaddr *)&from, fromlen);
  if (result == -1) {
    perror("Server sendto failed");
    exit (EXIT_FAILURE);
  }

  // close the socket and exit
  close(sfd);
  exit(0);
}// main

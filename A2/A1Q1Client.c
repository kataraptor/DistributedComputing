/*
 * A1Q1Client.c
 * COMP 3010 Distributed Computing
 * (C) Computer Science, University of Manitoba
 * Assignment 1 sample solution
 *
 */

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>           // for random numbers
#include <pthread.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>           // for sleep
#include <time.h>
typedef unsigned int uint;    // for return types

// Program constants
#define BUFSIZE 256                         // our buffer size
#define ENDCLIENT -1                        // client is done
#define MIN_REQUESTS 10                     // minimum requests
#define MAX_REQUESTS 20                     // maximum requests
#define MAX_INDEX 4000                      // maximum index of a request
#define MIN_DELAY 2                         // minimum delay in seconds
#define MAX_DELAY 5                         // maximum delay in seconds
#define NOT_FOUND "ERROR: LINE NOT FOUND"   // returned if index invalid

// a macro for random integers
#define RAND_INT(min,max) (rand()%((max)-(min)+1)+(min))



int main(int argc, char *argv[]) {
  int sfd;                // socket file descriptor
  uint retval;            // return result of function calls
  char buffer[BUFSIZE];   // buffer for fgets
  int numRequests;        // how many requests we will do
  int i;                  // loop index
  int request;            // the request sent to the server
  int delay;              // random delay in seconds
  int totalValid = 0;     // count of valid requests
  int totalInvalid = 0;   // count of invalid requests

  // check cmdline args
  if (argc < 3) {
    printf("Usage: %s <server> <port>\n", argv[0]);
    exit(0);
  }

  // set up the random number generator
  srand(6408169);   // seed the rng
  numRequests = RAND_INT(MIN_REQUESTS, MAX_REQUESTS);
  printf("Number of requests: %d\n", numRequests);

  /* create a socket

     Arguments:
       Domain (AF_INET): IPv4 Internet protocols
       Type (SOCK_STREAM): connection-based
       Protocol: (use 0 for IP)
   */
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    perror("client couldn't open a socket");
    exit (EXIT_FAILURE);
  }

  /* Use getaddrinfo to get the server's address information given the
     host/port entered on the command line. The result is returned in a
     linked list of addrinfo structs, shown below:
  */

  struct addrinfo *hostaddr;
  retval = getaddrinfo(argv[1], argv[2], NULL, &hostaddr);
  if(retval != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(sfd));
    exit(EXIT_FAILURE);
  }

  // attempt to connect to the server socket
  retval = connect(sfd, hostaddr->ai_addr, hostaddr->ai_addrlen);
  if (retval != 0) {
    perror("Couldn't connect to server");
    exit (EXIT_FAILURE);
  }
  for(i = 0; i < numRequests; i++){
    request = rand() % MAX_INDEX;
    delay = RAND_INT(MIN_DELAY, MAX_DELAY);
    sleep(delay);
    // let's write something to the socket
    bzero(buffer, BUFSIZE);
    sprintf(buffer, "%d", request);
    retval = write(sfd, buffer, strlen(buffer)); // write to a socket
    bzero(buffer,256);
    retval = read(sfd, buffer, BUFSIZE - 1);
    if(strcmp(NOT_FOUND, buffer) == 0){
      totalInvalid += 1;
    } else {
      totalValid += 1;
    }
    printf("%2d: %5d %s\n", i, request, buffer);
  }// for

  // tell the server we are done
    bzero(buffer, BUFSIZE);
    sprintf(buffer, "%d", ENDCLIENT);
    retval = write(sfd, buffer, strlen(buffer)); // write to a socket

  // close the socket
  close(sfd);

  // print statistics and exit
  printf("  Valid lines: %2d\n", totalValid);
  printf("Invalid lines: %2d\n", totalInvalid);
  exit(0);
}// main

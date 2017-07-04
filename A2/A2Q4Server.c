/*
 * A1Q1ServerV2.c
 * COMP 3010 Distributed Computing
 * (C) Computer Science, University of Manitoba
 * Assignment 1 sample solution
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>       // for inet_ntop
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <strings.h>
typedef unsigned int uint;   // to keep the compiler happy

// Global declarations

#define BUFSIZE 256                         // our buffer size
#define ENDCLIENT -1                        // client is done
#define NOT_FOUND "ERROR: LINE NOT FOUND"   // returned if index invalid

typedef struct file_info{    // struct returned by Directory server
  char name[80];
  int num_lines;
} FILE_INFO;

FILE_INFO info;               // the info from the directory server

typedef struct thread_arg{    // a struct you may use for A2Q4
  int sfd;                    // socket file descriptor
  struct in_addr client_ip;   // client address
} THREAD_ARG, *argptr;


char **file_lines;           // the lines read from the file
char buffer[BUFSIZE];        // buffer for I/O

//.............................................................................implementYourself
void multiThread(void* my_clientStruct){
  printf("we be here");
  //hahahahahaha
  printf("We be here");
  THREAD_ARG *clientStruct = (THREAD_ARG*)my_clientStruct;

  int newsock;               // file descriptor for incoming connection
  uint result;               // return result from functions
  int request = 0;           // client request

  // read requests and send replies until a negative number is received
  bzero(buffer, BUFSIZE);
  result = read(clientStruct->sfd, buffer, BUFSIZE - 1);
  request = atoi(buffer);
  while(request >= 0){
    printf("We received the message: %s\n", buffer);
    if(request < info.num_lines)
      result = write(clientStruct->sfd, file_lines[request], strlen(file_lines[request]) + 1);
    else
      result = write(clientStruct->sfd, NOT_FOUND, strlen(NOT_FOUND) + 1);
    bzero(buffer, BUFSIZE);
    result = read(clientStruct->sfd, buffer, BUFSIZE - 1);
    request = atoi(buffer);
  }
  close(clientStruct->sfd);
  printf("Closing connection to %s\n", inet_ntop(AF_INET, &clientStruct->client_ip, buffer, BUFSIZE));
}// implementYourself


//.............................................................................getFileInfo
FILE_INFO getFileInfo(){
  FILE_INFO result;          // what we will return
  FILE_INFO *f = &result;    // pointer for memcpy
  struct addrinfo *diraddr;  // directory address information
  int sfd;                   // socket file descriptor
  uint retval;               // return result from functions


  // set up address for Directory server
  retval = getaddrinfo("goose.cs.umanitoba.ca", "3010", NULL, &diraddr);
  if(retval != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(sfd));
    exit(EXIT_FAILURE);
  } else {
    printf("getFileInfo: getaddrinfo successful\n");
  }
  // create a datagram socket
  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1) {
    perror("Client couldn't open a socket");
    exit (1);
  } else {
    printf("getFileInfo: socket successful\n");
  }

  // send a datagram to the server and wait for a response
  bzero(buffer, BUFSIZE);
  retval = sendto(sfd, buffer, strlen(buffer), 0, diraddr->ai_addr, diraddr->ai_addrlen);
  if (retval == -1) {
    perror("getFileInfo: Directory server sendto failed");
    exit (1);
  } else {
    printf("getFileInfo: sendto successful\n");
  }

  retval = recvfrom(sfd, buffer, BUFSIZE, 0, NULL, NULL);
  if (retval == -1) {
    perror("getFileInfo: Directory server recvfrom failed");
    exit (1);
  } else {
    printf("getFileInfo: Directory server recvfrom successful\n");
    memcpy((void *)f, buffer, sizeof(FILE_INFO));
  }

  close(sfd);                   // close the datagram socket
  return result;
}// getFileInfo


void readFile(FILE_INFO info){
  int i;
  FILE *infile;
  char *line;
  printf("readFile: file name is %s, lines = %d\n", info.name, info.num_lines);

  file_lines = (char **)malloc(info.num_lines * sizeof(char *));
  // attempt to read file
  infile = fopen(info.name, "r");
  if(infile == NULL){
    perror("Error opening file");
    exit (EXIT_FAILURE);
  }

  // read the lines from the file
  i = 0;
  line = fgets(buffer, BUFSIZE, infile);
  while(line != NULL){
    file_lines[i++] = strdup(buffer);
    line = fgets(buffer, BUFSIZE, infile);
  }
  fclose(infile);
}// readFile

//....................................................................................main
int main(int argc, char *argv[]) {
  int sfd;                   // socket file descriptor
  int newsock;               // file descriptor for incoming connection
  uint result;               // return result from functions
  struct addrinfo hints;     // specify criteria for getaddrinfo
  struct addrinfo *hostaddr; // the host address information
  struct sockaddr_in caddr;  // client socket address
  uint clen = sizeof(caddr); // length of client address
  struct in_addr client_ip;  // client's ip address
  int request = 0;           // client request


  // check cmdline args
  if (argc < 2) {
    printf("Usage: %s <port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  printf("before file info");
  info = getFileInfo();
  readFile(info);

  // create a socket
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    perror("Server couldn't open a socket");
    exit (EXIT_FAILURE);
  }

  // set up a 'hints' struct to specify what kind of address we want
  bzero((void *)&hints, sizeof(hints));
  hints.ai_family = AF_INET;        // IPV4 address
  hints.ai_socktype = SOCK_STREAM;  // stream socket
  hints.ai_flags = AI_PASSIVE;      // suitable for accepting connections

  // get the addrinfo information for the server
  result = getaddrinfo(NULL, argv[1], &hints, &hostaddr);
  if(result != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(sfd));
    exit(EXIT_FAILURE);
  }

  printf("binding\n");
  // use bind to assign a name to the socket
  result = bind(sfd, hostaddr->ai_addr, hostaddr->ai_addrlen);
  printf("bind to %s\n", result);
  if (result == -1) {
    perror("Couldn't bind to server socket");
    exit (EXIT_FAILURE);
  }

  //printf("find arg %s", argv[1]);

  //printf("Stream server starting on port %s.\n", argv[1]);
  printf("Start\n");
  // loop forever
  int forever = 1;
  while(forever == 1){

    printf("We are in the while\n");
    // listen for connections to this socket
    result = listen(sfd, 5);
    if (result == -1) {
      perror("Listen failed");
      exit (EXIT_FAILURE);
    }

    printf("Found a connection\n");

    // accept a connection on this socket (creating a new connected socket)
    clen = sizeof(caddr);
    newsock = accept(sfd, (struct sockaddr *) &caddr, &clen);
    if (newsock==-1) {
      perror("Accept failed.");
      exit (EXIT_FAILURE);
    }
    // get the client's ip address
    client_ip = caddr.sin_addr;
    printf("Accepting connection from %s\n", inet_ntop(AF_INET, (void *)&client_ip, buffer, BUFSIZE));

    THREAD_ARG *clientStruct;
    clientStruct->client_ip = client_ip;
    clientStruct->sfd = newsock;

    pthread_t my_thread;
    printf("just before create");

    pthread_create(&my_thread, NULL, multiThread, (void*) clientStruct);

  }// end server

  // close the socket and exit
  printf("Server %s closing port %s\n", argv[0], argv[1]);
  close(sfd);
  exit(0);
}// main

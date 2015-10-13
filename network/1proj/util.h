// James Jessen, Adam Rodriguez, Forrest Weston
// Team: TCP
// Computer Networks - Project 1

#ifndef __UTIL_H__
#define __UTIL_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 256

typedef enum { false, true } bool;

int send_termed(const int socket, const char* msg);
char* recv_termed(const int socket);

int send_sized(const int socket, const char* msg);
char* recv_sized(const int socket);

void DieWithError(const int sock, const char* msg);
void DieWithUserMessage(const char *msg, const char *detail);
void DieWithSystemMessage(const char *msg);


#endif

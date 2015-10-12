// James Jessen

#ifndef __SHARED_H__
#define __SHARED_H__


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

void DieWithError(const int sock, const char* msg);
void DieWithUserMessage(const char *msg, const char *detail);
void DieWithSystemMessage(const char *msg);

int send_term(const int socket, const char* msg);
char* recv_term(const int socket);

int send_sized(const int socket, const char* msg);
char* recv_sized(const int socket);


#endif

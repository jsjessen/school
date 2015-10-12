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

void DieWithUserMessage(const char *msg, const char *detail);
void DieWithSystemMessage(const char *msg);

//int send_term(char* str);
//int recv_term();
//
//int send_sized(char* str);
//int recv_sized();


#endif

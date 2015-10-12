#include "shared.h"

// examples of major sections of code include handling the command line parameters; establishing the listening socket in the server; handling the input for the initial contact from the client; handling the interaction for the password interaction from the client.

 // You should also avoid repeating very similar pieces of code – instead move what would be repeated into functions with appropriate parameters. For example, there should be a function that reads a string that is prefixed by a length;there should be a function that reads a newline-terminated string; etc.


 
// Check return value of all syscalls
// print appropriate msg
// exit if error occurs

// remember that send and recv on TCP sockets do not necessarily send and receive as much data as was requested to be sent/received

// Make sure that buffers are big enough for the values that may be stored in them.

// use memcpy instead of strcpy

// send only neccesary char, not entire buf

// To go from a hostname or an IP address expressed in dotted decimal notation as a string to a value suitable for storing in a struct sockaddr_in for use in a call to connect you use the library function getaddrinfo()

// --------------------------------------------------

void DieWithError(const int sock, const char* msg)
{
    close(sock);
    fputs(msg, stderr);
    exit(1);
}

void DieWithUserMessage(const char *msg, const char *detail) 
{
    fputs(msg, stderr);
    fputs(": ", stderr);
    fputs(detail, stderr);
    fputc('\n', stderr);
    exit(1);
}

void DieWithSystemMessage(const char *msg) 
{
    perror(msg);
    exit(1);
}

// send msg  as a new-line-terminated (\n) string. Note that a null terminating character should not be sent when a newline-terminated string is specified.
int send_term(const int socket, const char* msg)
{
    char buf[BUFSIZE];
    int size = strlen(msg);

    strncpy(buf, msg, size);
    buf[size++] = '\n';
    
    if(send(socket, buf, size, 0) != size)
        DieWithError(socket, "send() sent a different number of bytes than expected");

    return size;
}

char* recv_term(const int socket)
{
    char buf[BUFSIZE];

    int size;
    if((size = recv(socket, buf, BUFSIZE, 0)) < 0)
        DieWithSystemMessage("recv() failed");

    char* msg;
    if((msg = malloc(size)) < 0)
        DieWithSystemMessage("recv_term: malloc() failed");

    memcpy(msg, buf, size);
    msg[size - 1] = '\0';
    return msg;
}


// This msg will be sent by first sending the length of the msg as a 2-byte binary number in network byte order (remember to use htons() on the sending side and ntohs() on the receiving side) followed by the msg string (not null terminated). No null term sent.
int send_sized(const int socket, const char* msg)
{
    char buf[BUFSIZE];
    char* msgBuf = buf + sizeof(uint16_t);
    uint16_t size = strlen(msg);
    uint16_t network_size;

    strncpy(msgBuf, msg, size);
    msgBuf[size++] = '\n';

    network_size = htons(size);
    memcpy(buf, &network_size, sizeof(uint16_t));
    
    if(send(socket, buf, size + sizeof(uint16_t), 0) != size + sizeof(uint16_t))
        DieWithError(socket, "send() sent a different number of bytes than expected");

    return size;
}

char* recv_sized(const int socket)
{
    char buf[BUFSIZE];
    char* msgBuf = buf + sizeof(uint16_t);
    uint16_t size;

    int n;
    if((n = recv(socket, buf, BUFSIZE, 0)) < 0)
        DieWithSystemMessage("recv() failed");

    size = *((uint16_t*)buf);
    size = ntohs(size);

    char* msg;
    if((msg = malloc(size)) < 0)
        DieWithSystemMessage("recv_term: malloc() failed");

    memcpy(msg, msgBuf, size);
    msg[size - 1] = '\0';
    return msg;
}

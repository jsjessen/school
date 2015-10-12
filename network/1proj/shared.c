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

void DieWithError(const char* msg)
{
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

/*
// send msg  as a new-line-terminated (\n) string. Note that a null terminating character should not be sent when a newline-terminated string is specified.
int send_term(char* str)
{
    char buf[SIZE];
    // len = length of message with term char 
    buf[len -1] = '\n';
    //...
    send(fd, buf, len);
}

int recv_term()
{
}


// This password will be sent to the server by first sending the length of the msg as a 2-byte binary number in network byte order (remember to use htons() on the sending side and ntohs() on the receiving side) followed by the msg string (not null terminated). No null term sent.
int send_sized(char* str)
{
    // This sends only the length, add code to send msg

    uint16_t length = 20;
    // convert and send
    network_byte_order = htons(length);
    send(fd, &network_byte_order, sizeof(uint16_t), 0);
}

int recv_sized()
{
    // receive and convert
    recv();
    ntohs(length);
}
*/

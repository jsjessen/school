// James Jessen, Adam Rodriguez, Forrest Weston
// Team: TCP
// Computer Networks - Project 1

#include "util.h"

// Send msg as a new-line-terminated (\n) string. Note that a null terminating character should not be sent when a newline-terminated string is specified.
int send_termed(const int socket, const char* msg)
{
    char buf[BUFSIZE];
    int size = strlen(msg);

    if(BUFSIZE < size + 1)
        DieWithError(socket, "String exceeds send_termed() buffer size");
    strncpy(buf, msg, size);
    buf[size++] = '\n';
    
    if(send(socket, buf, size, 0) != size)
        DieWithError(socket, "send() sent a different number of bytes than expected");

    return size;
}

// Receive a new-line-terminated string.
char* recv_termed(const int socket)
{
    char buf[BUFSIZE];
    int size = 0;

    // Data might not all arrive at once in a single packet, so loop
    while(true)
    {
        if((size += recv(socket, buf + size, BUFSIZE, 0)) < 0)
            DieWithSystemMessage("recv() failed");
        if(buf[size - 1] == '\n')
            break;
    }

    char* msg;
    if((msg = malloc(size)) < 0)
        DieWithSystemMessage("recv_termed: malloc() failed");

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

    if(BUFSIZE < size)
        DieWithError(socket, "String exceeds send_sized() buffer size");
    strncpy(msgBuf, msg, size);

    network_size = htons(size);
    memcpy(buf, &network_size, sizeof(uint16_t));
    size += sizeof(uint16_t);

    if(send(socket, buf, size, 0) != size)
        DieWithError(socket, "send() sent a different number of bytes than expected");

    return size;
}

// Receive string preceeded by 2-byte length.
char* recv_sized(const int socket)
{
    char buf[BUFSIZE];
    char* msgBuf = buf + sizeof(uint16_t);
    uint16_t size = 0;
    bool gotSize = false;

    int n = 0;
    // Data might not all arrive at once in a single packet, so loop
    while(n < size || !gotSize)
    {
        if((n += recv(socket, buf + n, BUFSIZE, 0)) < 0)
            DieWithSystemMessage("recv() failed");

        if(!gotSize && n >= sizeof(uint16_t))
        {
            size = *((uint16_t*)buf);
            size = ntohs(size);
            gotSize = true;
        }
    }

    char* msg;
    if((msg = malloc(size + 1)) < 0)
        DieWithSystemMessage("recv_termed: malloc() failed");
    memcpy(msg, msgBuf, size);
    msg[size] = '\0';
    return msg;
}


void DieWithError(const int sock, const char* msg)
{
    close(sock);
    fputs(msg, stderr);
    fputc('\n', stderr);
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

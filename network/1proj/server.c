// James Jessen
// Computer Networks - Project 1

// *** SERVER ***

#include "shared.h"

// Use new-line terminated strings

static const int MAXPENDING = 5; // Maximum outstanding connection requests

// Usage: Start me first
//  Port_to_listen 
int main(int argc, char* argv[])
{
    if (argc != 2) // Test for correct number of arguments
        DieWithUserMessage("Parameter(s)", "<Server Port>");

    in_port_t echoServPort = atoi(argv[1]); // First arg: local port

    // Create simple table for storing ID#'s and names
    // just an array of a few ID# and names hardcoded

    // Create socket for incoming connections
    int servSock;
    if((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    struct sockaddr_in echoServAddr; // Local address
    echoServAddr.sin_family = AF_INET;                // Internet address family
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
    echoServAddr.sin_port = htons(echoServPort);      // Local port

    // Assign a port to socket
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    // Mark the socket so it will listen for incoming connections
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    // Run until Ctrl-C signal
    while(1)
    {
        int clntSock;
        struct sockaddr_in echoClntAddr; // Client address
        int clntLen = sizeof(echoClntAddr);

        // Accept new connection
        if ((clntSock = accept(servSock,(struct sockaddr*)&echoClntAddr,&clntLen)) < 0)
            DieWithError("accept() failed");

        // ---------- Sever Blocked, Waiting for Connection from a Client ---------- 

        char echoBuffer[32];
        int recvMsgSize; 
        /* Receive message from client */
        if ((recvMsgSize = recv(clntSock, echoBuffer, BUFSIZE, 0)) < 0)
            DieWithError("recv() failed");
        printf("Message Received: '%s'\n", echoBuffer); 

        // Wait for client
        // send_term("Welcome"); but without Null char

        // recv_term ID
        // recv_term name
        // look up ID and name in table
        //
        // Validation
        // if found:
        //      send_term("Success");
        // else:
        //      send_term("Failure");
        //      close socket

        // recv_sized password
        // verify password matches the previously recveived name and ID number by checking table
        // if verified:
        //      send_sized "Congratulations <name>; you’ve just revealed the password for <ID number> to the world!"
        //  else:
        //      send_sized "Password incorrect"

        // close the connection
        close(clntSock);
    }
}

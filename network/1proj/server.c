// James Jessen, Adam Rodriguez, Forrest Weston
// Team: TCP
// Computer Networks - Project 1

// References: 
//
//      PracticalSocketC.ppt
//
//      Donahoo, Michael J., and Kenneth L. Calvert. 
//      TCP/IP sockets in C practical guide for programmers. 
//      Amsterdam Boston: Morgan Kaufmann, 2009. Print. 

#include "util.h"

typedef struct client
{
    char name[64];
    char pwd[64];
} Client;

static const int MAXPENDING = 4; // Maximum outstanding connection requests

// *** SERVER ***
int main(int argc, char* argv[])
{
    // Create simple table for storing ID#'s and names
    Client lookupTable[3];
    strcpy(lookupTable[0].name, "Adam");
    strcpy(lookupTable[0].pwd, "abc");
    strcpy(lookupTable[1].name, "Forrest");
    strcpy(lookupTable[1].pwd, "trees");
    strcpy(lookupTable[2].name, "James");
    strcpy(lookupTable[2].pwd, "bond");

    // Get port from commandline arguments
    if(argc != 2)
        DieWithUserMessage("Parameter(s)", "<Server Port>");
    in_port_t servPort = atoi(argv[1]);

    // Create socket for incoming connections
    int servSock;
    if((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError(servSock, "socket() failed");

    // Construct local address structure
    struct sockaddr_in servAddr; 
    memset(&servAddr, 0, sizeof(servAddr)); 
    servAddr.sin_family = AF_INET;                // Internet address family
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
    servAddr.sin_port = htons(servPort);          // Local port

    // Assign a port to socket
    if(bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithError(servSock, "bind() failed");

    // Mark the socket so it will listen for incoming connections
    if(listen(servSock, MAXPENDING) < 0)
        DieWithError(servSock, "listen() failed");

    // Run until Ctrl-C signal
    while(true)
    {
        // Accept new connection
        int clntSock;
        struct sockaddr_in clntAddr; 
        int clntLen = sizeof(clntAddr); 
        if((clntSock = accept(servSock,(struct sockaddr*)&clntAddr,&clntLen)) < 0)
            DieWithError(clntSock, "accept() failed");

        // ---------- Sever Blocked, Waiting for Connection from a Client ---------- 

        // Welcome client
        send_termed(clntSock, "Welcome");

        // Get client ID
        char* clientID_str = recv_termed(clntSock);
        printf("ID: '%s'\n", clientID_str);
        int clientID = atoi(clientID_str);
        free(clientID_str);

        // Get client Name
        char* clientName = recv_termed(clntSock);
        printf("Name: '%s'\n", clientName);

        // Look up ID and name in table  
        if(clientID >= 0 &&
                clientID < sizeof(lookupTable) / sizeof(Client) &&
                strcmp(lookupTable[clientID].name, clientName) == 0)
            send_termed(clntSock, "Success");
        else
        {
            send_termed(clntSock, "Failure");
            free(clientName);
            close(clntSock);
            continue;
        }

        // Verify password matches the previously recveived name and ID number
        char* clientPwd = recv_sized(clntSock);
        printf("Password: '%s'\n", clientPwd);
        if(strcmp(lookupTable[clientID].pwd, clientPwd) == 0)
        {
            char output[64];
            sprintf(output, "Congratulations %s; you’ve just revealed the " \
                    "password for %d to the world!", clientName, clientID);
            send_sized(clntSock, output); 
        }
        else
            send_sized(clntSock, "Password incorrect");

        free(clientPwd);
        free(clientName);

        // close the connection
        close(clntSock);
        clntSock = -1;
    }
}

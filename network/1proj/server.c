// James Jessen
// Computer Networks - Project 1

// *** SERVER ***

#include "shared.h"

static const int MAXPENDING = 4; // Maximum outstanding connection requests

struct Client
{
    char name[32];
    char pwd[32];
};

// Usage: Start me first
//  Port_to_listen 
int main(int argc, char* argv[])
{
    struct Client client_table[3];

    strcpy(client_table[0].name, "Adam");
    strcpy(client_table[0].pwd, "abc");

    strcpy(client_table[1].name, "Forrest");
    strcpy(client_table[1].pwd, "trees");

    strcpy(client_table[2].name, "James");
    strcpy(client_table[2].pwd, "hitch");

    // Create simple table for storing ID#'s and names
    // just an array of a few ID# and names hardcoded

    if (argc != 2) // Test for correct number of arguments
        DieWithUserMessage("Parameter(s)", "<Server Port>");

    in_port_t servPort = atoi(argv[1]); // First arg: local port

    // Create socket for incoming connections
    int servSock;
    if((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError(servSock, "socket() failed");

    // Construct local address structure
    struct sockaddr_in servAddr; // Local address
    memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure

    servAddr.sin_family = AF_INET;                // Internet address family
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
    servAddr.sin_port = htons(servPort);          // Local port

    // Assign a port to socket
    if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithError(servSock, "bind() failed");

    // Mark the socket so it will listen for incoming connections
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError(servSock, "listen() failed");

    // Run until Ctrl-C signal
    while(true)
    {
        struct sockaddr_in clntAddr; // Client address
        int clntLen = sizeof(clntAddr); // Set length of client address structure (in-out parameter)

        // Accept new connection
        int clntSock;
        if ((clntSock = accept(servSock,(struct sockaddr*)&clntAddr,&clntLen)) < 0)
            DieWithError(clntSock, "accept() failed");

        // ---------- Sever Blocked, Waiting for Connection from a Client ---------- 

        send_term(clntSock, "Welcome");

        char* clientID_str = recv_term(clntSock);
        
        int clientID = atoi(clientID_str);
        free(clientID_str);
        printf("ID: '%d'\n", clientID);
        char* clientName = recv_term(clntSock);
        printf("Name: '%s'\n", clientName);

        // Look up ID and name in table  
        if(clientID >= 0 &&
                clientID < sizeof(client_table) / sizeof(struct Client) &&
                strcmp(client_table[clientID].name, clientName) == 0)
            send_term(clntSock, "Success");
        else
        {
            send_term(clntSock, "Failure");
            free(clientName);
            close(clntSock);
            continue;
        }

        // verify password matches the previously recveived name and ID number
        char* clientPwd = recv_sized(clntSock);
        if(strcmp(client_table[clientID].pwd, clientPwd) == 0)
        {
            char output[64];
            sprintf(output, "Congratulations %s; you’ve just revealed the password for %d to the world!", clientName, clientID);
            send_sized(clntSock, output); 
        }
        else
            send_sized(clntSock, "Password incorrect");

        free(clientPwd);
        free(clientName);

        // close the connection
        close(clntSock);
    }
}

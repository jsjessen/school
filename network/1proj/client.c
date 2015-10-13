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

// *** CLIENT ***
int main(int argc, char* argv[])
{
    // Get server address and port from commandline arguments
    if(argc < 2 || argc > 3)
        DieWithUserMessage("Parameter(s)", "<Server Address> [<Server Port>]");
    char* servIP = argv[1]; // server IP address (dotted quad)
    in_port_t servPort = (argc == 3) ? atoi(argv[2]) : 51717;

    // Create a reliable, stream socket using TCP
    int sock;
    if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError(sock, "socket() failed");

    // Construct server address structure
    struct sockaddr_in servAddr; 
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family      = AF_INET;            // Internet address family
    servAddr.sin_addr.s_addr = inet_addr(servIP);  // Server IP address
    servAddr.sin_port        = htons(servPort);    // Server port

    // Connect to server 
    if(connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithError(sock, "connect() failed");

    // Listen for welcome message
    char* welcomeMsg = recv_termed(sock);
    puts(welcomeMsg);
    if(strcmp(welcomeMsg, "Welcome") != 0)
        DieWithError(sock, "Not Welcome :("); // let OS free msg
    free(welcomeMsg);

    // Prompt user for ID, send to server (assuming non-malicious user)
    char myID[32];
    printf("Enter ID: ");
    scanf("%s", myID);
    send_termed(sock, myID);

    // Prompt user Name, send to server (assuming non-malicious user)
    char myName[32];
    printf("Enter Name: ");
    scanf("%s", myName);
    send_termed(sock, myName);

    // Wait for server validation response
    char* validation = recv_termed(sock);
    printf("Validation(%s)\n", validation);
    if(strcmp(validation, "Success") != 0)
        DieWithError(sock, "Invalid ID/Name");
    free(validation);

    // Prompt user of Password, send to server (assuming non-malicious user)
    char myPwd[32];
    printf("Enter Password: ");
    scanf("%s", myPwd);
    send_sized(sock, myPwd);

    // Wait for server password response
    char* response = recv_sized(sock);
    printf("%s\n", response);
    free(response);

    // Close the connection
    close(sock);
    return 0;
}

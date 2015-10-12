// James Jessen
// Computer Networks - Project 1

// *** CLIENT ***

#include "shared.h"

// Usage: Start server first
//  Server_IP_Address Port_server_listening
int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in servAddr; // Server address

    if (argc < 2 || argc > 3) // Test for correct number of arguments
        DieWithUserMessage("Parameter(s)",
                "<Server Address> [<Server Port>]");

    char *servIP = argv[1]; // First arg: server IP address (dotted quad)
    in_port_t servPort = (argc == 3) ? atoi(argv[2]) : 51716;

    char* validation;
    do 
    {
        // Create a reliable, stream socket using TCP
        if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            DieWithError(sock, "socket() failed");

        memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
        servAddr.sin_family      = AF_INET;             // Internet address family
        servAddr.sin_addr.s_addr = inet_addr(servIP);   // Server IP address
        servAddr.sin_port        = htons(servPort); // Server port

        // Connect to server 
        if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
            DieWithError(sock, "connect() failed");

        // Listen for welcome msg
        char* welcomeMsg = recv_term(sock);
        puts(welcomeMsg);
        if(strcmp(welcomeMsg, "Welcome") != 0)
            DieWithError(sock, "Not Welcome :("); // let OS free msg
        free(welcomeMsg);

        // Get ID and Name
        // Assuming none malicious user 
        char myID[32];
        printf("Enter ID: ");
        scanf("%s", myID);
        send_term(sock, myID);

        char myName[32];
        printf("Enter Name: ");
        scanf("%s", myName);
        send_term(sock, myName);

        // wait for server response
        validation = recv_term(sock);
        printf("Validation: %s\n", validation);
        printf("--------------------\n");
    } 
    while(strcmp(validation, "Success") != 0);
    free(validation);

    // Password
    char myPwd[32];
    printf("Enter Password: ");
    scanf("%s", myPwd);
    send_sized(sock, myPwd);

    char* result = recv_sized(sock);
    printf("%s\n", result);
    free(result);

    // close the connection/socket
    close(sock);
    return 0;
}

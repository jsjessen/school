// James Jessen
// Computer Networks - Project 1

// *** CLIENT ***

#include "shared.h"

// Usage: Start server first
//  Server_IP_Address Port_server_listening
int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in echoServAddr; // Server address

    if (argc < 3 || argc > 4) // Test for correct number of arguments
        DieWithUserMessage("Parameter(s)",
                "<Server Address> <Echo Word> [<Server Port>]");

    char *servIP = argv[1]; // First arg: server IP address (dotted quad)
    char *echoString = argv[2]; // Second arg: string to echo
    in_port_t echoServPort = atoi(argv[3]);

    // Create a reliable, stream socket using TCP
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    echoServAddr.sin_family      = AF_INET;             // Internet address family
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   // Server IP address
    echoServAddr.sin_port        = htons(echoServPort); // Server port

    // Establish connection
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");

    //char* echoString = "Test Successful";
    int echoStringLen = strlen(echoString); // Determine input length
    // Send the string to the server
    if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
        DieWithError("send() sent a different number of bytes than expected");

    // Connect to server
    // Listen for welcome msg
    //      if not welcome:
    //          close connection, print error, exit

    // prompt user for numeric ID
    // get from stdin
    // prompt user for name 
    // get from stdin

    // send_term ID
    // send_term Name
    // wait for server response
    // recv_term validation response
    // if don't recv success:
    //      close (or try again?)

    // prompt user for password
    // send_sized password
    // recv_sized result
    
    // print result
    // close the connection/socket
    close(sock);
    // exit

    return 0;
}

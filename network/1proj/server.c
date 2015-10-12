// James Jessen
// Computer Networks - Project 1

// *** SERVER ***

#include "shared.h"

// Use new-line terminated strings

// Usage: Start me first
//  Port_to_listen 
int main(int argc, char* argv[])
{
    // Create simple table for storing ID#'s and names
    // just an array of a few ID# and names hardcoded
    
    // Loop
    {
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
        
        // close connection
    }
}
// Runs until Ctrl-C signal

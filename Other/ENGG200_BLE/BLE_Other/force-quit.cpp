//
//  main.cpp
//  ForceQuit
//
//  Created by Gayathri Rajan on 13/10/18.
//  Copyright © 2018 Gayathri Rajan. All rights reserved.
//

#include <iostream>
#include <string>

//message get
std::string getMessage() {
    std::string getM = "";
    std::cout << "Get message ";
    std::cin >> getM;
    return getM;
}

//function force quit
//Assuming that what is passed into forceQuit is a string message
//if first character in message is '9' then quit
int forceQuit(std::string m)
{
    if(*m.begin() == '9') {
        std::cout << "Force quit successful\n";
        exit(0);
    }
    else {
        // program continues
    }
    return 0;
}

// Calls forceQuit
void checkForceQuit(std::string m)
{
    std::cout << "message: " + m + " Checking message received for forceQuit\n";
    forceQuit(m);
}

// assume this is whole process fucntion
int exampleFunction ()
{
    std::string message = "";

    //assume function 1
    std::cout << "Step 1\n";
    message = getMessage();
    checkForceQuit(message);

    //assume function 2
    std::cout << "Step 2\n";
    message = getMessage();
    checkForceQuit(message);

    //assume function 3
    std::cout << "Step 3\n";
    message = getMessage();
    checkForceQuit(message);

    //assume function 4
    std::cout << "Step 4\n";
    message = getMessage();
    checkForceQuit(message);

    //assume function 5
    std::cout << "Step 5\n";
    message = getMessage();
    checkForceQuit(message);

    return 0;
}

int main(int argc, const char * argv[]) {
    //call main function here
    exampleFunction();
    return 0;
}

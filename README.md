# Comunication_Network
This repository contains some programs related to network.
portScanner.cpp is a simply port scanning program.
To run the program, Compile it with: g++ portScanner.cpp -pthread -o portScanner
Command Line argument is: ./portScanner start end IP1 ...
"start" is the beginning port number and end is the ending port number. IP1 is the first IP address that will get scanned. Multiple IP addresses can follow IP1 and they will be scanned with the same port range as IP1.

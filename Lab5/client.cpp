/*
Author: Yang Gu
Date last modified: 20/11/2023
Organization: ECE6122 Class

Description:
This C++ program uses the SFML Network library to create a TCP client that connects to a server, 
sends and receives messages based on user inputs. It supports message versioning, sending typed messages, 
and quitting the session. The program uses multi-threading to handle asynchronous message receiving.
*/

#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <stdio.h>
#include <string.h>
#include <chrono>

struct tcpMessage {
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

std::mutex coutMutex;

void receiveMessages(sf::TcpSocket& socket, std::atomic<bool>& running) {
    //socket.setBlocking(false);

    while (running) {
        tcpMessage msg;
        std::size_t received;
        if (socket.receive(&msg, sizeof(msg), received) != sf::Socket::Done) {
            break;
        }

        std::cout << "Received Msg Type: " << static_cast<int>(msg.nType) 
                << "; Msg: " << msg.chMsg << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <port_number>\n";
        return 1;
    }

    sf::TcpSocket socket;
    std::string serverIP = argv[1];
    unsigned short port = std::stoi(argv[2]);

    if (socket.connect(serverIP, port) != sf::Socket::Done) {
        std::cerr << "Connection to server failed" << std::endl;
        return 1;
    }

    std::atomic<bool> running(true);
    std::thread receiverThread(receiveMessages, std::ref(socket), std::ref(running));

    tcpMessage msg;
    msg.nVersion = 1; // Default version

    while (running) {
        std::string input;

        std::cout << "Please enter command: ";
        std::getline(std::cin, input);
        if (input.empty()) continue;

        if (input[0] == 'v') {
            msg.nVersion = std::stoi(input.substr(2));
        } else if (input[0] == 't') {
            size_t spacePos = input.find(' ', 2);
            msg.nType = std::stoi(input.substr(2, spacePos - 2));
            std::string message = input.substr(spacePos + 1);
            strncpy(msg.chMsg, message.c_str(), sizeof(msg.chMsg));
            msg.nMsgLen = static_cast<unsigned short>(message.size());

            if (socket.send(&msg, sizeof(msg)) != sf::Socket::Done) {
                std::cerr << "Failed to send message" << std::endl;
                running = false;
            }
        } else if (input[0] == 'q') {
            running = false;
            socket.disconnect();
        }
    
    }

    if (receiverThread.joinable()) {
        receiverThread.join();
    }

    return 0;
}

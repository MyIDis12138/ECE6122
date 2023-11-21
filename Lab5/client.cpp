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

std::mutex msgMutex;
tcpMessage lastReceivedMsg;

/**
 * Receives messages from the server and updates the last received message.
 * 
 * @param socket Reference to the TCP socket connected to the server.
 * @param running Atomic flag indicating if the client is still running.
 * @param receivedFlag Atomic flag that signals if a new message has been received.
 */
void receiveMessages(sf::TcpSocket& socket, std::atomic<bool>& running, std::atomic<bool>& receivedFlag) {
    socket.setBlocking(false);
    while (running) {
        tcpMessage msg;
        std::size_t receivedSize;

        if (socket.receive(&msg, sizeof(msg), receivedSize) != sf::Socket::Done) {
            continue;
        }

        {
            std::lock_guard<std::mutex> guard(msgMutex);
            lastReceivedMsg = msg;
            receivedFlag.store(true);
        }
    }
}

/**
 * Main function to start the client. Connects to the server, starts the message receiver
 * thread, and processes user commands to send messages or quit.
 * 
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return Returns 0 on successful execution, 1 on failure.
 */
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
    std::atomic<bool> received(false);
    std::thread receiverThread(receiveMessages, std::ref(socket), std::ref(running), std::ref(received));

    tcpMessage msg;
    msg.nVersion = 1; // Default version

    while (running) {
        std::string input;

        {        
            std::lock_guard<std::mutex> guard(msgMutex);
            received.load();
            if (received) {
                std::cout << "Received Msg Type: " << static_cast<int>(lastReceivedMsg.nType) 
                    << "; Msg: " << lastReceivedMsg.chMsg << std::endl;
                received.store(false);
            }
        }

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

            std::size_t sent = 0;

            if (socket.send(&msg, sizeof(msg), sent) != sf::Socket::Done) {
                std::cerr << "Failed to send message" << std::endl;
                running = false;
            }
        } else if (input[0] == 'q') {
            running = false;
            socket.disconnect();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));    
    }

    if (receiverThread.joinable()) {
        receiverThread.join();
    }

    return 0;
}

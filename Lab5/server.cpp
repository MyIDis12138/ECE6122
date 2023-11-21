/*
Author:  Yang Gu
Date last modified: 20/11/2023
Organization: ECE6122 Class

Description:
This C++ program is a TCP server using the SFML Network library. 
It accepts connections from multiple clients, handles client messages in separate threads, and supports various message operations. 
The server can broadcast messages, reverse and send back messages, and list connected clients.
It's designed to run continuously, processing client commands and managing client connections asynchronously. 
The server can be terminated through a command, closing all client connections gracefully.
*/

#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <condition_variable>

struct tcpMessage {
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

std::vector<sf::TcpSocket*> clients;
std::mutex clientsMutex;
tcpMessage lastReceivedMsg;
std::mutex lastMsgMutex;
std::atomic<bool> serverRunning(true);

/**
 * Handles communication with a connected client.
 * 
 * @param clientSocket Pointer to the socket connected to the client.
 * @param serverRunning Atomic flag indicating if the server is running.
 */
void handleClient(sf::TcpSocket* clientSocket, std::atomic<bool>& serverRunning) {
    while (serverRunning) {
        tcpMessage msg;
        std::size_t received;
        std::size_t sent;

        // Receive a message from the client
        if (clientSocket->receive(&msg, sizeof(msg), received) != sf::Socket::Done) {
            break;
        }

        {
            std::lock_guard<std::mutex> guard(lastMsgMutex);
            lastReceivedMsg = msg;
        }

        // Check message version
        if (msg.nVersion != 102) continue;

        // Handle different message types
        if (msg.nType == 77) { // Broadcast to all except sender
            std::lock_guard<std::mutex> guard(clientsMutex);
            for (auto& client : clients) {
                if (client != clientSocket) {
                    client->send(&msg, sizeof(msg), sent);
                }
            }
        } else if (msg.nType == 201) { // Reverse message and send back
            std::reverse(msg.chMsg, msg.chMsg + strlen(msg.chMsg));
            clientSocket->send(&msg, sizeof(msg), sent);
        }
    }

    // Clean up the client socket
    {
        std::lock_guard<std::mutex> guard(clientsMutex);
        auto it = std::find(clients.begin(), clients.end(), clientSocket);
        if (it != clients.end()) {
            clients.erase(it); 
        }
    }
    delete clientSocket;
}

/**
 * Accepts incoming client connections and spawns a new thread to handle each client.
 * 
 * @param listener Reference to the TCP listener for accepting new connections.
 * @param serverRunning Atomic flag indicating if the server is running.
 */
void acceptClients(sf::TcpListener& listener, std::atomic<bool>& serverRunning) {
    listener.setBlocking(false);

    while (serverRunning) {
        sf::TcpSocket* client = new sf::TcpSocket;
        if (listener.accept(*client) == sf::Socket::Done) {
            {
                std::lock_guard<std::mutex> guard(clientsMutex);
                clients.push_back(client);
            }
            std::thread(handleClient, client, std::ref(serverRunning)).detach();
        } else {
            delete client;
        }
    }
}

/**
 * Constructs and returns a string listing all connected clients.
 * 
 * @return A string containing the IP and port information of all connected clients.
 */
std::string getClientList() {
    std::lock_guard<std::mutex> guard(clientsMutex);
    std::stringstream ss;
    ss << "Number of Clients: " << clients.size() << "\n";
    for (auto& client : clients) {
        sf::IpAddress ip = client->getRemoteAddress();
        unsigned short port = client->getRemotePort();
        ss << "IP Address: " << ip << " | Port: " << port << "\n";
    }
    return ss.str();
}

/**
 * Closes all client connections and clears the clients list.
 */
void closeAllClients() {
    std::lock_guard<std::mutex> guard(clientsMutex);
    for (auto& client : clients) {
        client->disconnect();
        delete client;
    }
    clients.clear();
}

/**
 * Main function to start the server. It sets up the listener on a specified port,
 * starts the thread to accept clients, and processes server commands.
 * 
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return Returns 0 on successful execution, 1 on failure.
 */
int main(int argc, char* argv[]) {
    if (argc != 2){
        std::cerr << "Usage: " << argv[0] << " <port_number>\n";
        return 1;
    }

    sf::TcpListener listener;
    std::vector<std::thread> clientThreads;
    unsigned short port = std::stoi(argv[1]);

    if (listener.listen(port) != sf::Socket::Done) {
        std::cerr << "Error binding to port" << std::endl;
        return 1;
    }

    // Accept clients in a separate thread
    std::thread acceptThread(acceptClients, std::ref(listener), std::ref(serverRunning));
    
    // Command loop
    while (serverRunning) {
        std::string command;
        std::cout << "Please enter command: ";
        std::getline(std::cin, command);

        if (command == "msg") {
            std::lock_guard<std::mutex> guard(lastMsgMutex);
            std::cout << "Last Message: " << lastReceivedMsg.chMsg << "\n";
        } else if (command == "clients") {
            std::cout << getClientList();
        } else if (command == "exit") {
            serverRunning = false;
            listener.close(); 
        }
    }

    acceptThread.join();
    closeAllClients();

    return 0;
}
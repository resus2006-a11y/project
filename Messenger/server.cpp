#include <iostream>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
 
#define PORT        9000
#define BUFFER_SIZE 256
#define BACKLOG     5
 
int main() {
 
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        std::cerr << "Socket failed: " << strerror(errno) << std::endl;
        return errno;
    }
 
    int opt = 1;
    int reuseResult = setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (reuseResult == -1) {
        std::cerr << "Setsockopt failed: " << strerror(errno) << std::endl;
        close(serverFd);
        return errno;
    }
 
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port        = htons(PORT);  // htons converts to network byte order
 
    int bindResult = bind(serverFd, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
    if (bindResult == -1) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(serverFd);
        return errno;
    }
 
    int listenResult = listen(serverFd, BACKLOG);
    if (listenResult == -1) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(serverFd);
        return errno;
    }
 
    std::cout << "[Server] Listening on port " << PORT << std::endl;
 
    while (true) {
 
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
 
        int clientFd = accept(serverFd, (struct sockaddr*) &clientAddr, &clientLen);
        if (clientFd == -1) {
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue;
        }
 
        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, sizeof(clientIp));
        std::cout << "[Server] Client connected: "
                  << clientIp << ":" << ntohs(clientAddr.sin_port) << std::endl;
 
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
 
        if (bytesRead > 0) {
            std::cout << "[Server] Received (" << bytesRead << " bytes): " << buffer << std::endl;
        } else if (bytesRead == 0) {            std::cout << "[Server] Client disconnected without sending data." << std::endl;
        } else {
            std::cerr << "[Server] Recv failed: " << strerror(errno) << std::endl;
        }
 
        close(clientFd);
        std::cout << "[Server] Connection closed." << std::endl;
    }

    close(serverFd);
 
    return 0;
}

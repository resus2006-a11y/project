#include <iostream>
#innclude <cstring>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
 
#define PORT 9000
 
int main(int argc, char* argv[]) {

    const char* serverIp = (argc > 1) ? argv[1] : "127.0.0.1";
 
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd == -1) {
        std::cerr << "Socket failed: " << strerror(errno) << std::endl;
        return errno;
    }
 
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(PORT);
 
    int convResult = inet_pton(AF_INET, serverIp, &serverAddr.sin_addr);
    if (convResult != 1) {
        std::cerr << "Invalid address: " << serverIp << std::endl;
        close(sockFd);
        return 1;
    }
 
    int connectResult = connect(sockFd, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
    if (connectResult == -1) {
        std::cerr << "Connect failed: " << strerror(errno) << std::endl;
        close(sockFd);
        return errno;
    }
 
    std::cout << "[Client] Connected to " << serverIp << ":" << PORT << std::endl;
 
    const char* message = "Hello from client!";
 
    ssize_t bytesSent = send(sockFd, message, strlen(message), 0);
    if (bytesSent == -1) {
        std::cerr << "Send failed: " << strerror(errno) << std::endl;
        close(sockFd);
        return errno;
    }
 
    std::cout << "[Client] Sent " << bytesSent << " bytes: " << message << std::endl;
 
    close(sockFd);
    std::cout << "[Client] Done." << std::endl;
 
    return 0;
}

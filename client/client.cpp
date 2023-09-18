#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>

#define PORT 8080

using namespace std;

void receiver(int sock);

int main(int argc, char const* argv[]){
    cout << "CLIENT ON" << endl;

    int sock = 0, valread;
    struct sockaddr_in serv_addr;

    // Create Socket file descriptor
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket: ");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 ddresses from text to binary form
    const char* ipaddr = "127.0.0.1";
    if(inet_pton(AF_INET, ipaddr, &serv_addr.sin_addr) <= 0){
        perror("inet_pton: ");
        return -1;
    }

    // Connect to the server
    if(connect(sock, (struct sockaddr* )&serv_addr, sizeof(serv_addr)) < 0){
        perror("connect: ");
        return -1;
    }
/*
    // receiver thread
    thread th(receiver, sock);

    // Send Message
    while(1){
        char buffer[1024] = {0};
        cin >> buffer;
        send(sock, buffer, sizeof(buffer), 0);
    }
*/
    char buffer[1024] = {0, };
    cin >> buffer;
    send(sock, buffer, sizeof(buffer), 0);
    
    cout << "CLIENT OFF" << endl;
}

void receiver(int sock){
    while(1){
        char buffer[1024] = {0};
        recv(sock, buffer, 1024, 0);
        cout << "받음: " << buffer << endl;
    }
}
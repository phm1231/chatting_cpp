#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>

using namespace std;

void ReceiveFromServer(const int server_socket);
void InitClient(int& client_socket, struct sockaddr_in& address);

int main(int argc, char const* argv[]){
    
    int client_socket;
    struct sockaddr_in address;
    InitClient(client_socket, address);

    // receiver thread
    thread receiver(ReceiveFromServer, client_socket);
    receiver.detach();

    // Send Message
    while(true){
        char buffer[1024] = {0};
        cin >> buffer;
        send(client_socket, buffer, sizeof(buffer), 0);
    }

}

void InitClient(int& client_socket, struct sockaddr_in& address){

    const int kPORT = 8080;
    const char* server_address = "127.0.0.1";

    // Create Socket file descriptor
    assert((client_socket= socket(AF_INET, SOCK_STREAM, 0)) >= 0);

    address.sin_family = AF_INET;
    address.sin_port = htons(kPORT);

    // Convert IPv4 and IPv6 ddresses from text to binary form
    assert(inet_pton(AF_INET, server_address, &address.sin_addr) > 0);

    // Connect to the server
    assert(connect(client_socket, (struct sockaddr* )&address, sizeof(address)) >= 0);

}

void ReceiveFromServer(const int client_socket){
    while(1){
        char buffer[1024] = {0};
        recv(client_socket, buffer, 1024, 0);
        cout << "받음: " << buffer << endl;
    }
}
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

using namespace std;

void Receiver(const int sock);
void InitClient(int& client_socket, struct sockaddr_in& address);

int main(int argc, char const* argv[]){
    int client_socket;
    struct sockaddr_in address;
    InitClient(client_socket, address);

    thread th(Receiver, client_socket);

    while(1){
        char send_buffer[1024] = {0};
        cin >> send_buffer;
        send(client_socket, send_buffer, sizeof(send_buffer), 0);
    }
}

void InitClient(int& client_socket, struct sockaddr_in& address){
    const int kPORT = 8080;
    const char* server_address = "127.0.0.1";

    // Create Socket file descriptor
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    assert(client_socket >= 0);

    address.sin_family = AF_INET;
    address.sin_port = htons(kPORT);

    // Convert IPv4 and IPv6 ddresses from text to binary form
    int inet_pton_success = inet_pton(AF_INET, server_address, &address.sin_addr);
    assert(inet_pton_success > 0);

    // Connect to the server
    int connect_success = connect(client_socket, (struct sockaddr* )&address, sizeof(address));
    assert(connect_success >= 0);
}

void Receiver(const int sock){
    while (true) {
        char recv_buffer[1024] = {0};
        int read_byte = recv(sock, recv_buffer, sizeof(recv_buffer), 0);
        assert(read_byte > 0);
        cout << "받은 메세지: " << recv_buffer << endl;
    }
}
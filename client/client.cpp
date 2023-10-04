#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

using namespace std;

typedef struct Msg{
    int number;
    string str;
    Msg(){}
    Msg(int number, string str): number(number), str(str) {}
}Msg;

void InitClient(int& client_socket, struct sockaddr_in& address);

int main(int argc, char const* argv[]){
    int client_socket;
    struct sockaddr_in address;
    InitClient(client_socket, address);

    // client는 먼저 받는다.
    Msg recv_msg;
    recv(client_socket, &recv_msg, sizeof(recv_msg), 0);
    cout << "\n받은 number: " << recv_msg.number << endl;
    cout << "\n받은 str: " << recv_msg.str << endl;

    // 메세지를 보낸다.
    int num;
    string s;
    cout << "보낼 Num 입력: ";
    cin >> num;
    cout << "보낼 Str 입력: ";
    cin >> s;
    Msg send_msg(num, s);
    send(client_socket, &send_msg, sizeof(send_msg), 0);

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
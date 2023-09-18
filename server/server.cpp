#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <thread>
#include <string>
#include <cassert>
#include <algorithm>
#include <mutex>

using namespace std;

void BroadCastToClient(const string msg);
void ReceiveFromClient(const int client_socket);
void InitServer(int& server_socket, struct sockaddr_in& address);
void RemoveClient(const int client_socket);

vector<int> clients;
mutex clients_mutex;

int main(){
    int server_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    InitServer(server_socket, address);

    // Accept incoming connection
    while(1){
        int client_socket = accept(server_socket, (struct sockaddr* )&address, (socklen_t* )&addrlen);

        assert(client_socket >= 0);

        clients.push_back(client_socket); // 연결 대상에 추가

        thread receiver(ReceiveFromClient, client_socket); // 수신할 스레드 추가
        receiver.detach(); 
    }

    return 0;
}

void InitServer(int& server_socket, struct sockaddr_in& address){
    const int kPORT = 8080;
    int socket_opt = 1;
    // Creaating socket file descriptor
    assert((server_socket = socket(AF_INET, SOCK_STREAM, 0)) != 0);

    // Attaching socket to the port 8080
    assert(!setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &socket_opt, sizeof(socket_opt)));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(kPORT);

    // Bind the socket to prot
    assert(::bind(server_socket, (struct sockaddr* )&address, sizeof(address)) >= 0);

    // listen incomming connection
    assert(listen(server_socket, 3) >= 0);
}

void BroadCastToClient(const string msg){
    cout << "Lock in BroadCast" << endl;
    lock_guard<mutex> lg(clients_mutex);
    for(int client: clients){
        send(client, msg.c_str(), msg.size(), 0);
    }
}

void ReceiveFromClient(const int client_socket){
    try {
        while (true) {
            char buffer[1024] = {0};
            int read_byte = recv(client_socket, buffer, sizeof(buffer), 0);
            assert(read_byte >= 0);

            // 해당 Client의 접속 종료
            if(read_byte == 0){
                RemoveClient(client_socket);
                
                string out_msg = to_string(client_socket) + "님이 퇴장하였습니다.";
                BroadCastToClient(out_msg);
                break;
            }

            BroadCastToClient(string(buffer));
        }
    }
    catch (const std::exception& e) {
        cerr << "ReceiveFromClient: " << e.what() << endl;
    }
}

void RemoveClient(const int client_socket){
    // 클라이언트 삭제
    lock_guard<mutex> lg(clients_mutex);
    clients.erase(remove(clients.begin(), clients.end(), client_socket), clients.end());
}
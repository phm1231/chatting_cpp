#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <thread>
#include <string>
#include <cassert>
#include <algorithm>

using namespace std;

enum MSG_TYPE{
    READ_ROOM = 1,
    MAKE_ROOM,
    ENTER_ROOM
};

typedef struct Msg{
    MSG_TYPE msg_type;
    void* ptr = NULL;
    int siz;
    Msg(){}
    Msg(MSG_TYPE _msg_type, void* _ptr, int _siz){
        msg_type = _msg_type;
        siz = _siz;
        memcpy(ptr, _ptr, siz);
    }
}Msg;

void ReceiveFromClient(const int client_socket);
void InitServer(int& server_socket, struct sockaddr_in& address);
void HandleMsg(const Msg& msg, int client_socket);

int main(){
    int server_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    InitServer(server_socket, address);

    // Accept incoming connection
    while(1){
        int client_socket = accept(server_socket, (struct sockaddr* )&address, (socklen_t* )&addrlen);

        assert(client_socket >= 0);

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

void ReceiveFromClient(const int client_socket){
    try {
        while (true) {
            Msg recv_msg;
            int read_byte = recv(client_socket, &recv_msg, sizeof(recv_msg), 0);
            assert(read_byte >= 0);

            // 해당 Client의 접속 종료
            if(read_byte == 0) break;
            else HandleMsg(recv_msg, client_socket);
        }
    }
    catch (const std::exception& e) {
        cerr << "ReceiveFromClient: " << e.what() << endl;
    }
}

void HandleMsg(const Msg& msg, const int client_socket){
    int type = msg.type;
    switch(type){
        case 1:
            cout << "case 1" << endl;
            break;
        case 2:
            cout << "case 2" << endl;
            MakeRoom(msg.str, client_socket);
            break;
        case 3:
            cout << "case 3" << endl;
            cout << "참여할 방 이름: " << msg.str << endl;
            break;
        default:
            break;
    }
}

// 방을 만들고, 해당 방을 관리하는 프로세스를 생성해야 한다.
void MakeRoom(const string& room_name, const int client_socket){
    
}
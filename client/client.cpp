#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <cstring>

const int kPort = 8080;
const int kRoomPort = 8081;

enum MSG_TYPE{
    kWaitingMsg,
    kReadRoom,
    kMakeRoom,
    kEnterRoom
};

typedef struct Msg{
    MSG_TYPE type;
    size_t content_size;
    char content[128];
}Msg;

using namespace std;

void InitClient(int& client_socket, struct sockaddr_in& address);
void ExplainType();
Msg MsgInput(const int client_socket);
void ReceiveFromServer(const int client_socket);

int main(int argc, char const* argv[]){
    
    int client_socket;
    struct sockaddr_in address;
    InitClient(client_socket, address);

    thread receiver(ReceiveFromServer, client_socket);
    receiver.detach();

    while(1){
        const Msg send_msg = MsgInput(client_socket);
        send(client_socket, &send_msg, sizeof(send_msg), 0);
        
    }

    return 0;
}

void InitClient(int& client_socket, struct sockaddr_in& address){
    const char* server_address = "127.0.0.1";

    // Create Socket file descriptor
    assert((client_socket = socket(AF_INET, SOCK_STREAM, 0)) >= 0);

    address.sin_family = AF_INET;
    address.sin_port = htons(kPort);

    // Convert IPv4 and IPv6 ddresses from text to binary form
    assert(inet_pton(AF_INET, server_address, &address.sin_addr) > 0);

    // Connect to the server
    assert(connect(client_socket, (struct sockaddr* )&address, sizeof(address)) >= 0);
}

Msg MsgInput(const int client_socket){
    string room_name = "";
    Msg send_msg;

    ExplainType();

    int type;
    cout << "type 입력: ";
    cin >> type;

    send_msg.type = static_cast<MSG_TYPE>(type);

    switch(send_msg.type){
        case MSG_TYPE::kReadRoom: // 방 목록 요청
            break;
        case MSG_TYPE::kMakeRoom: // 방 만들기
            cout << "생성할 방 이름 입력: " << endl;
            cin >> send_msg.content;
            send_msg.content_size = strlen(send_msg.content);
            break;
        case MSG_TYPE::kEnterRoom: // 방 참여하기
            cout << "참여할 방 이름 입력: " << endl;
            cin >> send_msg.content;
            send_msg.content_size = strlen(send_msg.content);
            break;
        default:
            break;
    }

    return send_msg;
}

void ExplainType(){
    cout << "===== Explain Type =====" << endl;
    cout << "1. 방 목록 요청" << endl;
    cout << "2. 방 만들기" << endl;
    cout << "3. 방 참여하기" << endl;
    cout << "===== ============ =====" << endl;
}

void ReceiveFromServer(const int client_socket){
    try {
        while (true) {
            Msg recv_msg;
            char buffer[256];
            int read_byte = recv(client_socket, buffer, sizeof(recv_msg), 0);
            assert(read_byte >= 0);
            // 해당 Client의 접속 종료
            if(read_byte == 0) break;

            memcpy(&recv_msg, buffer, sizeof(recv_msg));
            cout << "받은 메세지: " << recv_msg.content << endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "ReceiveFromClient: " << e.what() << std::endl;
    }
}
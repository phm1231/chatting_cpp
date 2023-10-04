#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cassert>

using namespace std;
typedef struct Msg{
    int number;
    string str;
    Msg(){}
    Msg(int number, string str): number(number), str(str) {}
}Msg;

void InitServer(int& server_socket, struct sockaddr_in& address);

int main(){
    int server_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // 서버 소켓 설정
    InitServer(server_socket, address);

    // 클라이언트 연결 대기
    int client_socket = accept(server_socket, (struct sockaddr* )&address, (socklen_t* )&addrlen);
    assert(client_socket >= 0);

    // 보낼 메시지 입력 후 전송
    int num;
    string s;
    cout << "보낼 Num 입력: ";
    cin >> num;
    cout << "보낼 Str 입력: ";
    cin >> s;
    Msg send_msg(num, s);
    send(client_socket, &send_msg, sizeof(send_msg), 0);

    // 메시지를 수신할 때까지 대기, 수신 후 출력, 종료
    Msg recv_msg;
    recv(client_socket, &recv_msg, sizeof(recv_msg), 0);
    cout << "\n받은 number: " << recv_msg.number << endl;
    cout << "\n받은 str: " << recv_msg.str << endl;

    return 0;
}

void InitServer(int& server_socket, struct sockaddr_in& address){
    const int kPORT = 8080;
    int socket_opt = 1;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    assert(server_socket != 0);

    int setsockopt_success = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &socket_opt, sizeof(socket_opt));
    assert(!setsockopt_success);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(kPORT);

    int bind_success = ::bind(server_socket, (struct sockaddr* )&address, sizeof(address));
    assert(bind_success >= 0);

    int listen_success = listen(server_socket, 3);
    assert(listen_success >= 0);
}
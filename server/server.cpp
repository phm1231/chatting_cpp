#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cassert>

using namespace std;

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
    char send_buffer[1024] = {0};
    cin >> send_buffer;
    send(client_socket, send_buffer, sizeof(send_buffer), 0);

    // 메시지를 수신할 때까지 대기, 수신 후 출력, 종료
    char recv_buffer[1024] = {0};
    recv(client_socket, recv_buffer, sizeof(recv_buffer), 0);
    cout << "\n받은 메시지: " << recv_buffer << endl;

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
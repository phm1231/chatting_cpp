#include <server.hpp>
#include <iostream>

int main(){
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int server_socket = InitServer(address);

    while(1){
        int client_socket = accept(server_socket, (struct sockaddr* )&address, (socklen_t* )&addrlen);

        assert(client_socket >= 0);

        std::thread receiver(ReceiveFromClient, client_socket); // 수신할 스레드 추가
        receiver.detach(); 
    }

    return 0;
}

int InitServer(struct sockaddr_in& address){
    int socket_opt = 1;
    int server_socket;
    // Creaating socket file descriptor
    assert((server_socket = socket(AF_INET, SOCK_STREAM, 0)) != 0);

    // Attaching socket to the port 8080
    assert(!setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &socket_opt, sizeof(socket_opt)));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(kPort);

    // Bind the socket to prot
    assert(::bind(server_socket, (struct sockaddr* )&address, sizeof(address)) >= 0);

    // listen incomming connection
    assert(listen(server_socket, 3) >= 0);

    return server_socket;
}

void ReceiveFromClient(const int client_socket){
    try {
        while (true) {
            Msg recv_msg;
            char buffer[256];
            int read_byte = recv(client_socket, buffer, sizeof(recv_msg), 0);
            assert(read_byte >= 0);
            // 해당 Client의 접속 종료
            if(read_byte == 0) break;

            memcpy(&recv_msg, buffer, sizeof(recv_msg));
            HandleMsg(recv_msg);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "ReceiveFromClient: " << e.what() << std::endl;
    }
}

void HandleMsg(const Msg& msg){
    // type에 따라 content 내용을 정해서 리턴, fork로 방 만들기 처리 필요.
    switch(msg.type){
        case MSG_TYPE::kReadRoom:
            std::cout << "Read Room!" << std::endl;
            break;

        case MSG_TYPE::kMakeRoom:
            std::cout << "Make Room!" << std::endl;
            std::cout << "Room name is " << msg.content << std::endl;
            break;

        case MSG_TYPE::kEnterRoom:
            std::cout << "Enter Room!" << std::endl;
            std::cout << "Room name is " << msg.content << std::endl;
            break;

        default:
            break;
    }
}
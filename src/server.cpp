#include <server.hpp>

int main(){
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int server_socket;
    InitServer(server_socket, address);

    while(1){
        int client_socket = accept(server_socket, (struct sockaddr* )&address, (socklen_t* )&addrlen);
        assert(client_socket >= 0);

        std::thread receiver(ReceiveFromClient, client_socket); // 수신할 스레드 추가
        receiver.detach(); 
    }
}

void InitServer(int& server_socket, struct sockaddr_in& address){
    int socket_opt = 1;
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
}

void ReceiveFromClient(const int client_socket){
    try {
        while (true) {
            char buffer[512] = {0};
            int read_byte = recv(client_socket, buffer, sizeof(buffer), 0);
            assert(read_byte >= 0);
            // 해당 Client의 접속 종료
            if(read_byte == 0){
                Disconnect(client_socket);
                break;
            }
            Msg recv_msg;
            memcpy(&recv_msg, buffer, sizeof(recv_msg));
            std::cout << "Recv From " << recv_msg.header.sender << std::endl;
            HandleRecvMsg(recv_msg, client_socket);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "ReceiveFromClient: " << e.what() << std::endl;
    }
}

void HandleRecvMsg(const Msg& recv_msg, const int& client_socket){
    const std::string& room_name = recv_msg.header.room_name;

    switch(recv_msg.header.type){
        case MSG_TYPE::kEnterRoom:
            std::cout << "EnterRoom" << std::endl;
            if(!CheckRoomExist(room_name)) break; // 방이 없다면
            EnterRoom(room_name, client_socket);
            break;

        case MSG_TYPE::kCreateRoom:
            std::cout << "CreateRoom" << std::endl;
            if(CheckRoomExist(room_name)) break; // 동일한 이름의 방이 이미 존재한다면
            EnterRoom(room_name, client_socket);
            break;

        case MSG_TYPE::kReadRoom:

            break;

        case MSG_TYPE::kSendMsg:
            std::cout << "SendMsg" << std::endl;
            BroadCastMsg(recv_msg, client_socket);
            break;

        default:
            break;
    }
}

bool CheckRoomExist(const std::string& room_name){
    std::shared_lock<std::shared_mutex> lock(room_locks[room_name]);
    return rooms.count(room_name) == 1;
}

void DeleteRoom(const std::string& room_name){
    std::unique_lock<std::shared_mutex> lock(room_locks[room_name]);
    if(rooms[room_name].size() == 0) rooms.erase(room_name);

}

void EnterRoom(const std::string& room_name, const int& client_socket){
    std::unique_lock<std::shared_mutex> lock(room_locks[room_name]);
    rooms[room_name].emplace_back(client_socket);
}

void ExitRoom(const std::string& room_name, const int& client_socket){
    std::unique_lock<std::shared_mutex> lock(room_locks[room_name]);
    rooms[room_name].erase(std::remove(rooms[room_name].begin(), rooms[room_name].end(), client_socket), rooms[room_name].end());
    if(rooms[room_name].size() == 0){
        lock.unlock();
        DeleteRoom(room_name);
    }
}

Msg GetList(){
    // room_list의 길이가 너무 길지는 않은지 확인해주어야 함.
    // return Msg(MsgHeader(MSG_TYPE::kReadRoom, "", ""), room_list);    
}

void BroadCastMsg(const Msg& send_msg, const int& client_socket){
    const std::string& room_name = send_msg.header.room_name;
    std::shared_lock<std::shared_mutex> lock(room_locks[room_name]);
    for(int client: rooms[room_name]){
        std::cout << "client: " << client << std::endl;
        if(client == client_socket) continue;
        send(client, &send_msg, sizeof(send_msg), 0);
    }
}

void Disconnect(const int& client_socket){
    // ExitRoom(client_socket);
    // BroadCastMsg();
}
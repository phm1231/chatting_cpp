#include <server.hpp>

std::unordered_map<std::string, std::vector<int> > room_and_clients; // <room_name, sockets>
const std::string kRoomNameDuplication = "이미 존재하는 방 제목입니다.";
const std::string kMakeRoomSuccess = "방이 생성되었습니다.";
const std::string kEnterRoomSuccess = "방에 입장하였습니다.";
const std::string kEnterRoomFailed = "입장에 실패하였습니다.";

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

// 1개의 client로부터 메세지를 받는 함수
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
            HandleMsg(recv_msg, client_socket);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "ReceiveFromClient: " << e.what() << std::endl;
    }
}

void HandleMsg(const Msg& recv_msg, const int client_socket){
    // type에 따라 content 내용을 정해서 리턴, fork로 방 만들기 처리 필요.
    Msg send_msg;
    send_msg.type = recv_msg.type;

    switch(recv_msg.type){
        case MSG_TYPE::kReadRoom:{
            std::string room_names = GetRoomNames();
            std::copy(room_names.begin(), room_names.end(), send_msg.content);
            send_msg.content_size = strlen(send_msg.content);
            send(client_socket, &send_msg, sizeof(send_msg), 0);
            break;
        }

        case MSG_TYPE::kMakeRoom:{
            std::string room_name(recv_msg.content);
            if(room_and_clients.count(room_name) > 0){
                std::copy(kRoomNameDuplication.begin(), kRoomNameDuplication.end(), send_msg.content);
                send_msg.content_size = strlen(send_msg.content);
                send(client_socket, &send_msg, sizeof(send_msg), 0);
            }
            else{
                room_and_clients[room_name].push_back(client_socket);
                std::copy(kMakeRoomSuccess.begin(), kMakeRoomSuccess.end(), send_msg.content);
                send_msg.content_size = strlen(send_msg.content);
                send(client_socket, &send_msg, sizeof(send_msg), 0);
            }
            break;
        }
        case MSG_TYPE::kEnterRoom:{
            std::string room_name(recv_msg.content);
            if(room_and_clients.count(room_name) == 0){
                std::copy(kEnterRoomFailed.begin(), kEnterRoomFailed.end(), send_msg.content);
                send_msg.content_size = strlen(send_msg.content);
                send(client_socket, &send_msg, sizeof(send_msg), 0);
            }
            break;
        }
        default:
            break;
    }
}

std::string GetRoomNames(){
    std::string ret = "";
    for(auto iter = room_and_clients.begin(); iter != room_and_clients.end(); iter++) ret += iter->first + "$";
    ret += "\0";
    return ret;
}

void BroadCastToClient(const Msg& msg){
    /*
    shared_lock<shared_mutex> lock(clients_mutex);
    for(int client: room_and_client[clients){
        send(client, msg.c_str(), msg.size(), 0);
    }
    */
}
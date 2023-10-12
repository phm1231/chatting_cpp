#include <server.hpp>

int main(){
    Server server;
    // MsgQ를 처리할 thread 생성
    server.AddWorkThread();

    // 새로운 client의 연결 대기
    while(1){
        int client_socket = server.Accept();
        assert(client_socket >= 0);
        // client 전용 recv thread 추가
        server.AddClientReceiver(client_socket);
        // client 전용 Id를 client_socket으로 부여함.
        Msg send_msg({MSG_TYPE::kSetSockId, client_socket, ""}, "");
        send(client_socket, &send_msg, sizeof(send_msg), 0);
    }
}

Server::Server(){
    int socket_opt = 1;
    addr_len = sizeof(address);
    // Creaating socket file descriptor
    assert((sockfd = socket(AF_INET, SOCK_STREAM, 0)) != 0);
    // Attaching socket to the port 8080
    assert(!setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &socket_opt, sizeof(socket_opt)));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(kPort);
    // Bind the socket to port
    assert(::bind(sockfd, (struct sockaddr* )&address, sizeof(address)) >= 0);
    // listen incomming connection
    assert(listen(sockfd, 3) >= 0);
}

void Server::AddWorkThread(){
    // Server로 들어오는 요청을 처리하는 Thread 생성
    thread msg_consumer(&Server::MsgConsumer, this);
    msg_consumer.detach();
}

int Server::Accept(){
    return accept(sockfd, (struct sockaddr*)&address, (socklen_t* )&addr_len);
}

void Server::AddClientReceiver(const int& client_socket){
    thread receiver(&Server::MsgProducer, this, client_socket);
    receiver.detach();
}

void Server::MsgProducer(const int& client_socket){
    try {
        while (1) {
            char buffer[512] = {0, };
            int read_byte = recv(client_socket, buffer, sizeof(buffer), 0);
            assert(read_byte >= 0);
            // 해당 Client의 접속 종료
            if(read_byte == 0){
                MsgPush(Msg({{MSG_TYPE::kDisconnected, client_socket, GetId(client_socket)}, ""}));
                close(client_socket);
                break;
            }

            Msg recv_msg;
            memcpy(&recv_msg, buffer, sizeof(recv_msg));
            MsgPush(recv_msg);
        }
    }
    catch (const exception& e) {
        cerr << "MsgProducer: " << e.what() << endl;
    }
}

void Server::MsgConsumer(){
    while(1){
        if(MsgQEmpty()) continue;

        const Msg msg = MsgPop();
        HandleRecvMsg(msg);
    }
}

void Server::MsgPush(const Msg& msg){
    unique_lock<shared_mutex> lock(msg_q_lock);
    msg_q.push(msg);
}

Msg Server::MsgPop(){
    unique_lock<shared_mutex> lock(msg_q_lock);
    Msg msg = msg_q.front();
    msg_q.pop();

    return msg;
}

bool Server::MsgQEmpty(){
    shared_lock<shared_mutex> lock(msg_q_lock);
    return msg_q.empty();
}

void Server::SetRoomName(const int& client_socket, const string& room_name){
    unique_lock<shared_mutex> lock(client_lock);
    client_infos[client_socket].SetRoomName(room_name);
}

string Server::GetRoomName(const int& client_socket){
    shared_lock<shared_mutex> lock(client_lock);
    return client_infos[client_socket].GetRoomName();
}

void Server::SetId(const int& client_socket, const string& _id){
    unique_lock<shared_mutex> lock(client_lock);
    client_infos[client_socket].SetId(_id);
}

string Server::GetId(const int& client_socket){
    shared_lock<shared_mutex> lock(client_lock);
    return client_infos[client_socket].GetId();
}

void Server::HandleRecvMsg(const Msg& recv_msg){
    const MSG_TYPE msg_type = recv_msg.header.type;
    
    switch(msg_type){
        // 최초 연결 시 client에게 Id 할당
        case MSG_TYPE::kSetSockId:{
            SendSockId(recv_msg);
            break;
        }
        // 현재 만들어진 방의 List 전달
        case MSG_TYPE::kReadRoom:{
            SendList(recv_msg);
            break;
        }
        // Enter 요청 처리
        case MSG_TYPE::kEnterRoom:{
            EnterRoom(recv_msg);
            BroadCastMsg(Msg({{recv_msg.header}, "입장하였습니다."})); // 입장 알림
            break;
        }
        // Leave 요청 처리
        case MSG_TYPE::kLeaveRoom:{
            BroadCastMsg(Msg({{recv_msg.header}, "퇴장하였습니다."})); // 퇴장 알림
            LeaveRoom(recv_msg);
            break;
        }
        // Send 요청 처리
        case MSG_TYPE::kSendMsg:{
            BroadCastMsg(recv_msg);
            break;
        }
        // 종료 요청 처리
        case MSG_TYPE::kDisconnected:{
            Disconnect(recv_msg);
        }
        // Login 요청 처리
        case MSG_TYPE::kLogin:{
            Login(recv_msg);
            break;
        }
        // Logout 요청 처리
        case MSG_TYPE::kLogout:{
            Logout(recv_msg);
            break;
        }
        default:
            break;
    }
}

void Server::SendSockId(const Msg& msg){
    send(msg.header.client_socket, &msg, sizeof(msg), 0);
}

void Server::EnterRoom(const Msg& msg){
    const int client_socket = msg.header.client_socket;
    const string room_name(msg.content);
    SetRoomName(client_socket, room_name);

    unique_lock<shared_mutex> lock(room_lock);
    rooms[room_name].emplace_back(client_socket);
}

void Server::LeaveRoom(const Msg& msg){
    const int client_socket = msg.header.client_socket;
    string room_name = GetRoomName(client_socket);
    unique_lock<shared_mutex> lock(room_lock);
    rooms[room_name].erase(remove(rooms[room_name].begin(), rooms[room_name].end(), client_socket), rooms[room_name].end());
    if(rooms[room_name].size() == 0) rooms.erase(room_name);
    SetRoomName(msg.header.client_socket, "");
}

void Server::Login(const Msg& msg){
    const int client_socket = msg.header.client_socket;
    const string id(msg.header.sender);
    SetId(client_socket, id);
}

void Server::Logout(const Msg& msg){
    const int client_socket = msg.header.client_socket;
    unique_lock<shared_mutex> lock(client_lock);
    client_infos.erase(client_socket);
}

void Server::BroadCastMsg(const Msg& msg){
    const int client_socket = msg.header.client_socket;
    string room_name = GetRoomName(client_socket);
    for(int client: rooms[room_name]){
        if(msg.header.type == MSG_TYPE::kSendMsg && client == client_socket) continue;
        else send(client, &msg, sizeof(msg), 0);
    }
}

void Server::SendList(const Msg& msg){
    const int client_socket = msg.header.client_socket;
    string lists = GetList();
    Msg send_msg({{MSG_TYPE::kSendMsg, client_socket, "SERVER"}, lists});
    send(client_socket, &send_msg, sizeof(send_msg), 0);
}

void Server::Disconnect(const Msg& msg){
    LeaveRoom(msg);
    Logout(msg);
}

string Server::GetList(){
    shared_lock<shared_mutex> lock(room_lock);
    string lists = "현재 방 목록\n";
    for(auto iter = rooms.begin(); iter != rooms.end(); iter++){
        lists += iter->first + "\n";
    }
    return lists;
}
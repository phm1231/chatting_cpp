#include "client.hpp"

int main(int argc, char const* argv[]){
    
    while(!is_login) Login();

    struct sockaddr_in address;
    InitClient(sockfd, address);

    std::thread receiver(ReceiveFromServer);
    receiver.detach();
    Help();

    while(1){
        const std::string user_input = GetUserInput();
        HandleUserInput(user_input);
    }

    return 0;
}

inline std::string GetUserInput(){
    std::string user_input = "";
    getline(std::cin, user_input);
    return trim(user_input);
}

void HandleUserInput(const std::string& user_input){
    if(user_input == "/Login") Login();
    else if(user_input == "/Logout") Logout();
    else if(user_input == "/Help") Help();
    else if(user_input == "/List") GetList();
    else if(user_input == "/Enter") EnterRoom();
    else if(user_input == "/Create") CreateRoom();
    else SendMsg(user_input);
}

void Login(){
    if(is_login) return;

    std::cout << "ID: ";
    std::string _id;
    std::cin >> _id;
    id = trim(_id);
    is_login = true;
    std::cout << "Login Success\n";
}

void Logout(){
    if(!is_login) return;
    id = "";
    is_login = false;
    std::cout << "Logout Success\n";
}

void Help(){
    std::cout << "\n===== 사용설명서 =====\n";
    std::cout << "/Help\n";
    std::cout << "/Login\n";
    std::cout << "/Logout\n";
    std::cout << "/List\n";
    std::cout << "/Enter\n";
    std::cout << "/Create\n";
    std::cout << "/Exit\n";
    std::cout << "===== ===== ===== =====\n";
}

void GetList(){
    if(!is_login || has_joined_room) return;

    Msg send_msg(MsgHeader(MSG_TYPE::kReadRoom, id, ""), "");
    send(sockfd, &send_msg, sizeof(send_msg), 0);
}

void EnterRoom(){
    if(!is_login || has_joined_room) return;

    std::string enter_room = "";
    std::cout << "참여할 방 이름 입력: ";

    getline(std::cin, enter_room);
    enter_room = trim(enter_room);
    assert(CheckInputValidity(enter_room, CHECK_TYPE::kRoomName));

    Msg send_msg(MsgHeader(MSG_TYPE::kEnterRoom, id, enter_room), "");
    send(sockfd, &send_msg, sizeof(send_msg), 0);
}

void CreateRoom(){
    if(!is_login || has_joined_room) return;

    std::string create_room = "";
    std::cout << "생성할 방 이름 입력: ";
    getline(std::cin, create_room);

    create_room = trim(create_room);
    assert(CheckInputValidity(create_room, CHECK_TYPE::kRoomName));

    Msg send_msg(MsgHeader(MSG_TYPE::kCreateRoom, id, create_room), "");
    send(sockfd, &send_msg, sizeof(send_msg), 0);
}

void SendMsg(const std::string& user_input){
    assert(CheckInputValidity(user_input, CHECK_TYPE::kContent));
    Msg send_msg(MsgHeader(MSG_TYPE::kSendMsg, id, room_name), user_input);
    send(sockfd, &send_msg, sizeof(send_msg), 0);
}

inline bool CheckInputValidity(const std::string& str, CHECK_TYPE check_type){
    return str.size() < kCheckLength[check_type];
}

void ReceiveFromServer(){
    while(1){
        char recv_buffer[512] = {0, };
        recv(sockfd, recv_buffer, sizeof(recv_buffer), 0);

        Msg recv_msg;
        memcpy(&recv_msg, recv_buffer, sizeof(recv_msg));
        HandleRecvMsg(recv_msg);
    }    
}

void HandleRecvMsg(const Msg& recv_msg){
    switch(recv_msg.header.type){
        case MSG_TYPE::kEnterRoom:
            if(is_login && !has_joined_room){
                room_name = recv_msg.header.room_name;
                std::cout << "Enter: " << room_name << std::endl;
            }
            break;
        case MSG_TYPE::kCreateRoom:
            if(is_login && !has_joined_room){
                room_name = recv_msg.header.room_name;
                std::cout << "Create: " << room_name << std::endl;
            }
            break;
        case MSG_TYPE::kReadRoom:
            if(is_login && !has_joined_room){
                std::cout << "List: " << recv_msg.content << std::endl;
            }
            break;
        case MSG_TYPE::kSendMsg:
            if(is_login && has_joined_room){
                std::cout << recv_msg.header.sender << ": " << recv_msg.content << std::endl;
            }
            break;
        default:
            break;
    }
}

std::string& ltrim(std::string &s)
{
    auto it = std::find_if(s.begin(), s.end(),
                    [](char c) {
                        return !std::isspace<char>(c, std::locale::classic());
                    });
    s.erase(s.begin(), it);
    return s;
}

std::string& rtrim(std::string &s)
{
    auto it = std::find_if(s.rbegin(), s.rend(),
                    [](char c) {
                        return !std::isspace<char>(c, std::locale::classic());
                    });
    s.erase(it.base(), s.end());
    return s;
}

std::string& trim(std::string &s) {
    return ltrim(rtrim(s));
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

    std::cout << id << " Server Connected\n";
}

#include "client.hpp"

int main(){    
    Client client;
    client.AddServerReceiver();

    while(!client.is_login) client.Login();
    client.Help();

    while(1){
        const string user_input = client.GetUserInput();
        client.HandleUserInput(user_input);
    }

    return 0;
}

Client::Client(){
    const char* server_address = "127.0.0.1";
    // Create Socket file descriptor
    assert((sockfd = socket(AF_INET, SOCK_STREAM, 0)) >= 0);
    address.sin_family = AF_INET;
    address.sin_port = htons(kPort);
    // Convert IPv4 and IPv6 ddresses from text to binary form
    assert(inet_pton(AF_INET, server_address, &address.sin_addr) > 0);
    // Connect to the server
    assert(connect(sockfd, (struct sockaddr* )&address, sizeof(address)) >= 0);

    GetSockId();
}

void Client::GetSockId(){
    while(1){
        char buffer[512] = {0, };
        recv(sockfd, buffer, sizeof(buffer), 0);
        Msg recv_msg;
        memcpy(&recv_msg, buffer, sizeof(recv_msg));
        if(recv_msg.header.type == MSG_TYPE::kSetSockId){
            int &non_const_sock_id = const_cast<int&>(sock_id);
            non_const_sock_id = recv_msg.header.client_socket;
            is_login = false;
            has_joined_room = false;
            break;
        }
    } 
}

void Client::AddServerReceiver(){
    thread receiver(&Client::ReceiveFromServer, this);
    receiver.detach();
}

void Client::ReceiveFromServer(){
    try{
        while(1){
            char recv_buffer[512] = {0, };
            int read_byte = recv(sockfd, recv_buffer, sizeof(recv_buffer), 0);
            assert(read_byte >= 0);

            if(read_byte == 0){
                cout << "read_byte is 0, Server Err" << endl;
                exit(0);
            }

            Msg recv_msg;
            memcpy(&recv_msg, recv_buffer, sizeof(recv_msg));
            cout << recv_msg.header.sender << ": " << recv_msg.content << endl;
        }   
    }
    catch (const exception& e) {
        cerr << "ReceiveFromServer: " << e.what() << endl;
    } 
}

void Client::Login(){
    if(is_login){
        cout << "이미 로그인이 되어있습니다." << endl;
        return;
    }

    cout << "사용할 ID 입력: ";
    string id = GetUserInput();

    id = trim(id);
    if(id == "") return;

    client_info.SetId(id);
    Msg send_msg({{MSG_TYPE::kLogin, sock_id, client_info.GetId()}, ""});
    send(sockfd, &send_msg, sizeof(send_msg), 0);
    is_login = true;
    cout << "로그인에 성공하였습니다." << endl;
}

void Client::Logout(){
    if(!is_login){
        cout << "로그인이 필요합니다." << endl;
        return;
    }
    if(has_joined_room){
        cout << "방에 참여한 상태로 로그아웃할 수 없습니다." << endl;
        return;
    }
    Msg send_msg({{MSG_TYPE::kLogout, sock_id, client_info.GetId()}, ""});
    send(sockfd, &send_msg, sizeof(send_msg), 0);
    client_info.SetId("");
    is_login = false;

    cout << "로그아웃에 성공하였습니다.\n";
}

inline string Client::GetUserInput(){
    string user_input = "";
    getline(cin, user_input);
    trim(user_input);
    return user_input;
}

void Client::HandleUserInput(const string& user_input){
    if(user_input == "/Login") Login();
    else if(user_input == "/Logout") Logout();
    else if(user_input == "/Help") Help();
    else if(user_input == "/List") RequestRoomList();
    else if(user_input == "/Enter") EnterRoom();
    else if(user_input == "/Leave") LeaveRoom();
    else if(user_input == "/Exit") Exit();
    else SendMsg(user_input);
}

inline void Client::Help(){
    cout << "\n===== 명령어 목록 =====\n";
    cout << "/Help\n";
    cout << "/Login\n";
    cout << "/Logout\n";
    cout << "/List\n";
    cout << "/Enter\n";
    cout << "/Leave\n";
    cout << "/Exit\n";
    cout << "===== ===== ===== =====\n";
}

void Client::RequestRoomList(){
    Msg send_msg({{MSG_TYPE::kReadRoom, sock_id, client_info.GetId()}, ""});
    send(sockfd, &send_msg, sizeof(send_msg), 0);
}

void Client::EnterRoom(){
    if(!is_login){
        cout << "로그인이 필요합니다." << endl;
        return;
    }
    if(has_joined_room){
        cout << "이미 방에 참여 중입니다." << endl;
        return;
    }

    string enter_room = "";
    cout << "참여할 방 이름: ";
    enter_room = GetUserInput();

    client_info.SetRoomName(enter_room);
    has_joined_room = true;

    SendRequest(Msg{{MSG_TYPE::kEnterRoom, sock_id, client_info.GetId()}, client_info.GetRoomName()});
}

void Client::LeaveRoom(){
    if(!is_login){
        cout << "로그인이 필요합니다." << endl;
        return;
    }
    if(!has_joined_room){
        cout << "참여 중인 방이 없습니다." << endl;
        return;
    }

    has_joined_room = false;
    SendRequest(Msg({MSG_TYPE::kLeaveRoom, sock_id, client_info.GetId()}, ""));
    client_info.SetRoomName("");
    cout << "방에서 퇴장합니다." << endl;
}

inline void Client::SendRequest(const Msg& send_msg){
    send(sockfd, &send_msg, sizeof(send_msg), 0);
}

void Client::SendMsg(const string& user_input){
    SendRequest(Msg({MSG_TYPE::kSendMsg, sock_id, client_info.GetId()}, user_input));
}

void Client::Exit(){
    SendRequest(Msg({MSG_TYPE::kDisconnected, sock_id, client_info.GetId()}, ""));
    close(sockfd);
    cout << "프로그램을 종료합니다." << endl;
    exit(0);
}

string& ltrim(string &s)
{
    auto it = find_if(s.begin(), s.end(),
                    [](char c) {
                        return !isspace<char>(c, locale::classic());
                    });
    s.erase(s.begin(), it);
    return s;
}

string& rtrim(string &s)
{
    auto it = find_if(s.rbegin(), s.rend(),
                    [](char c) {
                        return !isspace<char>(c, locale::classic());
                    });
    s.erase(it.base(), s.end());
    return s;
}

string& trim(string &s) {
    return ltrim(rtrim(s));
}
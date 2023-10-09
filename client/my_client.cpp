#include "client.hpp"

int main(int argc, char const* argv[]){
    
    while(!is_login) Login();
    Help();

    int client_socket;
    struct sockaddr_in address;
    InitClient(client_socket, address);

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
    else if(user_input == "/Make") MakeRoom();
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
    std::cout << "/Enter (room_name)\n";
    std::cout << "/Make (room_name)\n";
    std::cout << "/Exit\n";
    std::cout << "===== ===== ===== =====\n";
}

void GetList(){

}

void EnterRoom(){
    if(!is_login || has_joined_room) return;

    std::string join_room = "";
    std::cout << "참여할 방 이름 입력: ";
    getline(std::cin, join_room);
    join_room = trim(join_room);

    Msg send_msg(MsgHeader(MSG_TYPE::kEnterRoom, id, join_room), "");
    send()
}

void MakeRoom(){

}
void SendMsg(const std::string& user_input){

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

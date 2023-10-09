#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <cstring>

const int kPort = 8080;
bool is_login = false;
bool has_joined_room = false;
std::string room_name = "";
std::string id = "";

void InitClient(int& client_socket, struct sockaddr_in& address);
inline std::string GetUserInput();
void HandleUserInput(const std::string& user_input);
void Login();
void Logout();
void Help();
void GetList();
void EnterRoom();
void MakeRoom();
void SendMsg(const std::string& user_input);

std::string& ltrim(std::string &s);
std::string& rtrim(std::string &s);
std::string& trim(std::string &s);

enum MSG_TYPE{
    kWaitingMsg,
    kReadRoom,
    kMakeRoom,
    kEnterRoom,
    kSendMsg
};

class MsgHeader{
private:
    MSG_TYPE type;
    std::string sender;
    std::string room_name;

public:
    MsgHeader(const MSG_TYPE type, const std::string& sender, const std::string& room_name):
        type(type), sender(sender), room_name(room_name) {}
};

class Msg{
public:
    MsgHeader header;
    std::string content;

    Msg(const MsgHeader& header, const string& content): header(header), content(content);
}
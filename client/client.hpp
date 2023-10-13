#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <cstring>
#include <thread>
#include "types.hpp"

using namespace std;

const int kPort = 8080;

class Client{
private:
    int sockfd;
    struct sockaddr_in address;
    const int sock_id = -1; // Server에서 사용하는 별도의 식별자.
    ClientInfo client_info; // <login_id, 참여한 방 이름>

    void GetSockId(); // 최초 Server와 연결 시 sock_id 할당

    void ReceiveFromServer();

    // 옵션 별 기능 구현
    void Logout();
    void RequestRoomList();
    void EnterRoom();
    void LeaveRoom();
    void SendMsg(const string& user_input);
    void Exit();

    // msg를 send
    inline void SendRequest(const Msg& send_msg);

public:
    bool is_login;
    bool has_joined_room;

    Client();
    void AddServerReceiver();

    // 사용자 입력 및 입력에 따른 함수 호출
    inline string GetUserInput();
    void HandleUserInput(const string& user_input);

    // 옵션 별 기능 구현
    void Login();
    inline void Help();

};

string& ltrim(string &s);
string& rtrim(string &s);
string& trim(string &s);

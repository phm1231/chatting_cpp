#include "types.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <cassert>
#include <thread>
#include <shared_mutex>
#include <unordered_map>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <unistd.h>

using namespace std;

const int kPort = 8080;

typedef unordered_map<string, vector<int> > Rooms;
typedef unordered_map<int, ClientInfo > ClientInfos;

class Server{
private:
    int sockfd, addr_len;
    struct sockaddr_in address;

    // MsgQ를 통한 Request 관리
    queue<Msg> msg_q;
    shared_mutex msg_q_lock;
    void MsgConsumer();
    void MsgProducer(const int& client_socket);
    void MsgPush(const Msg& msg);
    Msg MsgPop();
    bool MsgQEmpty();

    // MsgQ에서 뽑은 메세지를 처리
    void HandleRecvMsg(const Msg& recv_msg);
    void BroadCastMsg(const Msg& msg);

    // Client 정보(id, room_name) 관리
    ClientInfos client_infos;    
    shared_mutex client_lock;
    string GetRoomName(const int& client_socket);
    void SetRoomName(const int& client_socket, const string& room_name);
    string GetId(const int& client_socket);
    void SetId(const int& client_socket, const string& _id);
    // Client의 socket 번호를 식별자로 활용할 수 있도록 최초 Client 연결 시 Client에게 식별자 제공.
    void SendSockId(const Msg& msg);

    // Room 및 Client 정보 관리(<방이름, 참여한 클라이언트 목록>)
    Rooms rooms;
    shared_mutex room_lock;
    void EnterRoom(const Msg& msg);
    void LeaveRoom(const Msg& msg);
    void Login(const Msg& msg);
    void Logout(const Msg& msg);
    void Disconnect(const Msg& msg);
    void SendList(const Msg& msg);
    string GetList();

public:
    Server();
    int Accept();
    void AddWorkThread();
    void AddClientReceiver(const int& client_socket);
};
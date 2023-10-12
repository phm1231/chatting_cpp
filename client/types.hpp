// include/types.hpp와 동일한 파일임.

#include <string>

enum MSG_TYPE{
    kSetSockId,
    kReadRoom,
    kEnterRoom,
    kLeaveRoom,
    kSendMsg,
    kDisconnected,
    kLogin,
    kLogout
};

class MsgHeader{
public:
    MSG_TYPE type;
    int client_socket;
    char sender[32];

    MsgHeader(){}

    MsgHeader(const MSG_TYPE _type, const int& _client_socket, const std::string& _sender){
        type = _type;
        client_socket = _client_socket;
        strcpy(sender, _sender.c_str());
    }
};

class Msg{
public:
    MsgHeader header;
    char content[256];

    Msg(){}

    Msg(const MsgHeader& _header, const std::string& _content){
        header = _header;
        strcpy(content, _content.c_str());
    }
};

class ClientInfo{
private:
    std::string id;
    std::string room_name;
public:
    ClientInfo(const std::string& _id = "", const std::string& _room_name = ""){
        SetId(_id);
        SetRoomName(_room_name);
    }
    std::string GetId(){
        return id;
    }
    void SetId(const std::string& _id){
        id = _id;
    }
    std::string GetRoomName(){
        return room_name;
    }
    void SetRoomName(const std::string& _room_name){
        room_name = _room_name;
    }
};
#include <string>

enum CHECK_TYPE{
    kUserId,
    kRoomName,
    kContent
};

enum MSG_TYPE{
    kReadRoom,
    kCreateRoom,
    kEnterRoom,
    kSendMsg,
    kExitRoom
};

class MsgHeader{
public:
    MSG_TYPE type;
    char sender[32];
    char room_name[32];
    
    MsgHeader(){}

    MsgHeader(const MSG_TYPE _type, const std::string& _sender, const std::string& _room_name){
        type = _type;
        std::copy(_sender.begin(), _sender.end(), sender);
        std::copy(_room_name.begin(), _room_name.end(), room_name);
    }
};

class Msg{
public:
    MsgHeader header;
    char content[256];

    Msg(){}

    Msg(const MsgHeader& _header, const std::string& _content){
        header = _header;
        std::copy(_content.begin(), _content.end(), content);
    }
};

class Client{
private:
    std::string id;
    std::string room_name;
public:
    Client(const std::string& _id = "", const std::string& _room_name = ""){
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
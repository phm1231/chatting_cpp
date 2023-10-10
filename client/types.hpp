#include <string>

enum CHECK_TYPE{
    kUserId,
    kRoomName,
    kContent
};

enum MSG_TYPE{
    kWaitingMsg,
    kReadRoom,
    kCreateRoom,
    kEnterRoom,
    kSendMsg
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
#include <cstdlib>
#include <cstring>

enum MSG_TYPE{
    kReadRoom = 1,
    kMakeRoom,
    kEnterRoom
};

typedef struct Msg{
    MSG_TYPE msg_type;
    size_t msg_siz;
    void* content = NULL;

    Msg(const MSG_TYPE& _msg_type, const size_t _msg_siz, const void* _content){
        msg_type = _msg_type;
        msg_siz = _msg_siz;
        content = new char[msg_siz];
        memcpy(content, _content, msg_siz);
    }
    ~Msg(){
        if(content != NULL){
            char* tmp = static_cast<char* >(content);
            delete[] tmp;
        }
    }

}Msg;

typedef struct ReadRoomMsg{

}ReadRoomMsg;

typedef struct MakeRoomMsg{

}MakeRoomMsg;

typedef struct EnterRoomMsg{

}EnterRoomMsg;
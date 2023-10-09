#include <cstdlib>
#include <cstring>

// #define RecvMsg SendMsg 추후에 타입을 나누어서 수정해야 할 것(직렬화 관련)

const int kPort = 8080;

enum MSG_TYPE{
    kWaitingMsg,
    kReadRoom,
    kMakeRoom,
    kEnterRoom,
    kSendMsg
};

typedef struct Msg{
    MSG_TYPE type;
    size_t content_size;
    char content[128];
}Msg;
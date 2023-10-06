#include <cstdlib>
#include <cstring>

const int kPort = 8080;
const int kRoomPort = 8081;

enum MSG_TYPE{
    kWaitingMsg,
    kReadRoom,
    kMakeRoom,
    kEnterRoom
};

typedef struct Msg{
    MSG_TYPE type;
    size_t content_size;
    char content[128];
}Msg;
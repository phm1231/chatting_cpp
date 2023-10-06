#include <types.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cassert>
#include <thread>
#include <shared_mutex>


int InitServer(struct sockaddr_in& address);
void ReceiveFromClient(const int client_socket);
void HandleMsg(const Msg& msg);

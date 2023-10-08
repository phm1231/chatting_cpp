#include <types.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cassert>
#include <thread>
#include <shared_mutex>
#include <unordered_map>
#include <iostream>
#include <string>
#include <vector>

int InitServer(struct sockaddr_in& address);
void ReceiveFromClient(const int client_socket);
void HandleMsg(const Msg& msg, const int client_socket);
void BroadCastToClient(const Msg& msg);
std::string GetRoomNames();
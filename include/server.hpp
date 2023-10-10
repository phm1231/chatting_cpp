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

typedef std::unordered_map<std::string, std::vector<int> > Rooms;
typedef std::unordered_map<std::string, std::shared_mutex > RoomLocks;

Rooms rooms;
RoomLocks room_locks;

// std::vector<std::string> room_names;
// std::shared_mutex room_names_lock;

const int kPort = 8080;

void InitServer(int& server_socket, struct sockaddr_in& address);
void ReceiveFromClient(const int client_socket);
void HandleRecvMsg(const Msg& recv_msg, const int& client_socket);
bool CheckRoomExist(const std::string& room_name);
void DeleteRoom(const std::string& room_name);
void EnterRoom(const std::string& room_name, const int& client_socket);
void ExitRoom(const std::string& room_name, const int& client_socket);
Msg GetList();
void BroadCastMsg(const Msg& send_msg, const int& client_socket);
void Disconnect(const int& client_socket);
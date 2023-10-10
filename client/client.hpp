#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <cstring>
#include <thread>
#include "types.hpp"

const int kPort = 8080;
bool is_login = false;
bool has_joined_room = false;
std::string room_name = "";
std::string id = "";
int sockfd;
const int kCheckLength[] = {32, 32, 256};

void InitClient(int& client_socket, struct sockaddr_in& address);
inline std::string GetUserInput();
void HandleUserInput(const std::string& user_input);
void Login();
void Logout();
void Help();
void GetList();
void EnterRoom();
void CreateRoom();
void SendMsg(const std::string& user_input);
inline bool CheckInputValidity(const std::string& str, CHECK_TYPE check_type);
void ReceiveFromServer();
void HandleRecvMsg(const Msg& recv_msg);

std::string& ltrim(std::string &s);
std::string& rtrim(std::string &s);
std::string& trim(std::string &s);


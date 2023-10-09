#include <iostream>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

using namespace std;

const int kPort = 8080;

void InitClient(int& client_socket, struct sockaddr_in& address){
    const char* server_address = "127.0.0.1";

    // Create Socket file descriptor
    assert((client_socket = socket(AF_INET, SOCK_STREAM, 0)) >= 0);

    address.sin_family = AF_INET;
    address.sin_port = htons(kPort);

    // Convert IPv4 and IPv6 ddresses from text to binary form
    assert(inet_pton(AF_INET, server_address, &address.sin_addr) > 0);

    // Connect to the server
    assert(connect(client_socket, (struct sockaddr* )&address, sizeof(address)) >= 0);
}


class Test{
public:
    char hello[256];
    char content[256];
    
    Test(){}
    Test(const string& h, const string c){
        strcpy(hello, h.c_str());
        strcpy(content, c.c_str());
    }
};

int main(){
    int client_socket;
    struct sockaddr_in address;
    InitClient(client_socket, address);
    cout << "siz: " << sizeof(Test) << endl;
    while(1){
        string s1, s2;
        cin >> s1;
        cin >> s2;
        Test t2(s1 , s2);
        send(client_socket, &t2, sizeof(t2), 0);

    }
}
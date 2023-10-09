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

int InitServer(struct sockaddr_in& address){
    int socket_opt = 1;
    int server_socket;
    // Creaating socket file descriptor
    assert((server_socket = socket(AF_INET, SOCK_STREAM, 0)) != 0);

    // Attaching socket to the port 8080
    assert(!setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &socket_opt, sizeof(socket_opt)));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(kPort);

    // Bind the socket to prot
    assert(::bind(server_socket, (struct sockaddr* )&address, sizeof(address)) >= 0);

    // listen incomming connection
    assert(listen(server_socket, 3) >= 0);

    return server_socket;
}

int main(){
    
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int server_socket = InitServer(address);
    int client_socket = accept(server_socket, (struct sockaddr* )&address, (socklen_t* )&addrlen);
    assert(client_socket >= 0);

    while(1){
        Test t;
        int read_byte = recv(client_socket, &t, sizeof(t), 0);
        assert(read_byte >= 0);
        cout << "Hello: " << t.hello << endl;
        cout << "Content: " << t.content << endl;
    }

    return 0;
}

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <thread>

using namespace std;

void notifyAll(const char* message);
void receiver(int sock);
vector<int> sockets;

const int PORT = 8080;

int main(){
    cout << "SERVER ON" << endl;

    int server_fd, opt = 1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creaating socket file descriptor
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket: ");
        return -1;
    }

    // Attaching socket to the port 8080
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        perror("setsockopt: ");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to prot
    if(::bind(server_fd, (struct sockaddr* )&address, sizeof(address)) < 0){
        perror("bind: ");
        return -1;
    }

    // listen incomming connection
    if(listen(server_fd, 3) < 0){
        perror("listen: ");
        return -1;
    }

    // Accept incoming connection
    while(1){
        int new_socket = accept(server_fd, (struct sockaddr* )&address, (socklen_t* )&addrlen);
        cout << "accept: " << new_socket << endl;
        if(new_socket < 0){
            perror("accept: ");
            return -1;
        }
        sockets.push_back(new_socket); // 관리 대상에 등록
        thread th(receiver, new_socket); // receiver 추가
    }

    cout << "SERVER OFF" << endl;

    return 0;
}

void notifyAll(const char* message){
    for(int sock: sockets){
        send(sock, message, 1024, 0);
    }
}

// 서버는 수신한 후에 다른 모두에게 뿌려주어야 함.
void receiver(int sock){
    cout << "thread receiver: " << sock << endl;
    try {
        while (1) {
            char buffer[1024] = {0};
            int bytesRead = recv(sock, buffer, 1024, 0);
            if (bytesRead <= 0) {
                cout << "BytesRead <= 0" << endl;
                break;
            }
            cout << "Server received: " << buffer << endl;
            // notifyAll(buffer);
        }
    } catch (const std::exception& e) {
        cerr << "Exception in receiver: " << e.what() << endl;
    }
}
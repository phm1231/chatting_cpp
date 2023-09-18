#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

const int PORT = 8080;
const int MAX_CLIENTS = 10;

vector<int> clients;
mutex clientsMutex;

void broadcastMessage(int sender, const string& message) {
    lock_guard<mutex> lock(clientsMutex);
    for (int client : clients) {
        if (client != sender) {
            send(client, message.c_str(), message.size(), 0);
        }
    }
}

void handleClient(int clientSocket) {
    char buffer[1024];
    while (true) {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            // 클라이언트 연결 종료
            lock_guard<mutex> lock(clientsMutex);
            auto it = find(clients.begin(), clients.end(), clientSocket);
            if (it != clients.end()) {
                clients.erase(it);
            }
            close(clientSocket);
            break;
        }
        buffer[bytesRead] = '\0';
        string message(buffer);
        cout << "Received: " << message << endl;
        broadcastMessage(clientSocket, message);
    }
}

int main() {
    int serverSocket, opt = 1;
    int clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // 서버 소켓 생성
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cerr << "Socket creation error" << endl;
        return -1;
    }

    // Attaching socket to the port 8080
    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        perror("setsockopt: ");
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // 서버 바인딩
    if (::bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Bind error" << endl;
        return -1;
    }

    // 서버 리스닝
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        cerr << "Listen error" << endl;
        return -1;
    }

    cout << "Server is listening on port " << PORT << endl;

    while (true) {
        // 클라이언트 연결 대기
        if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
            cerr << "Accept error" << endl;
            continue;
        }

        lock_guard<mutex> lock(clientsMutex);
        clients.push_back(clientSocket);

        // 클라이언트 핸들링을 위한 스레드 시작
        thread clientThread(handleClient, clientSocket);
        // clientThread.detach(); // 클라이언트 스레드는 데몬 스레드로 실행

        cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << endl;
    }

    close(serverSocket);

    return 0;
}

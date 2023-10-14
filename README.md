### C++과 Thread로 구현한 다중 채팅방 Server&Client
___
#### 명령어 정리
| Command | Description |
| ------ | ----------- |
| /Login   | ID를 입력하여 로그인한다.|
| /Logout   | 계정에서 로그아웃한다.|
| /Enter   | 채팅방 이름을 입력하여 새 채팅방을 생성하거나 기존 채팅방에 입장한다.|
| /Leave   | 현재 접속한 채팅방에서 퇴장한다.|
| /Help   | 명령어를 출력한다.|
| /List   | 현재 개설된 채팅방 목록을 보여준다.|
| /Exit   | 서버와 연결을 종료하고 프로그램을 종료한다.|
___

#### Server 실행 방법
    git clone https://github.com/phm1231/chatting_cpp.git
    cd chatting_cpp
    make
    ./bin/main

#### Client 실행 방법
    cd client
    g++ client.cpp -o client
    ./client


[블로그 정리글](https://lotto12.tistory.com/22)

#include <thread>
#include <iostream>

using namespace std;

void work(int here);

int main(){
    thread th1(work, 1);
    thread th2(work, 2);
    thread th3(work, 3);

    th1.join();
    th2.join();

    return 0;
}

void work(int here){
    cout << here << endl;
}
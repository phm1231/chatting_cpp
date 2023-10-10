#include <iostream>
#include <vector>
#include <unordered_map>
#include <shared_mutex>


using namespace std;

shared_mutex locker;

void func2(){
    unique_lock<shared_mutex> lock(locker);
    cout << "Here is func2" << endl;
}

void func1(){
    unique_lock<shared_mutex> lock(locker);
    cout << "Here is func1" << endl;
    func2();
    return;
}

int main(){
    func1();
    return 0;
}
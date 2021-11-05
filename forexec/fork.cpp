#include <iostream>
#include <unistd.h>
#include<sys/wait.h>

using namespace std;

void print_pids(const string& str) {
    cout << str << ": this process pid - " << getpid() << "\n";
    cout << str << ": parent process pid - " << getppid() << "\n";
}

int main() {
    print_pids("before fork");
    int child = fork();
    if(child == 0) {
        print_pids("child");
    } else  {
        print_pids("parent");
        wait(nullptr);
    }
}

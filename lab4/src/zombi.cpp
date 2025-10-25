#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        cout << "fork error\n";
        return 1;
    }

    if (pid == 0) {
        cout << "child pid = " << getpid() << endl;
        cout << "child done\n";
        _exit(0);
    } else {
        cout << "parent pid = " << getpid() << ", child pid = " << pid << endl;
        cout << "sleep 10 sec, check ps output\n";
        sleep(10);
        cout << "parent done\n";
    }

    return 0;
}
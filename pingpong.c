#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 1) {
        exit(1);
    }

    int p[2], c[2];

    // 创建两个管道，p 用于父进程到子进程的通信，c 用于子进程到父进程的通信
    if (pipe(p) == -1 || pipe(c) == -1) {
        exit(2);
    }

    int pid = fork();

    if (pid < 0) {
        exit(3);
    } else if (pid > 0) {  // 父进程
        int p_pid = getpid();
        char msg[1];

        // 父进程向管道 p 的写入端写入一个字节 "H"
        write(p[1], "H", 1);

        // 父进程从管道 c 的读取端读取一个字节
        read(c[0], msg, 1);

        // 打印父进程接收到的信息
        fprintf(1, "%d: received pong\n", p_pid);
    } else {  // 子进程
        int c_pid = getpid();
        char msg[1];

        // 子进程从管道 p 的读取端读取一个字节
        read(p[0], msg, 1);

        // 打印子进程接收到的信息
        fprintf(1, "%d: received ping\n", c_pid);

        // 子进程向管道 c 的写入端写入一个字节 "H"
        write(c[1], "H", 1);
    }

    // 关闭所有管道
    close(p[0]);
    close(p[1]);
    close(c[0]);
    close(c[1]);

    exit(0);
}

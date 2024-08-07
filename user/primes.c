#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 过滤函数，读取左侧管道的数并进行素数筛选
void filter(int left_fd);

int main(int argc, char *argv[]) {
    int p[2];
    // 创建父进程到第一个子进程的管道
    pipe(p);

    if (fork() == 0) {
        // 子进程
        close(p[1]); // 关闭写端
        filter(p[0]); // 调用过滤函数处理管道读取
    } else {
        // 父进程
        close(p[0]); // 关闭读端
        // 向管道写入数字2到35
        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(i));
        }
        close(p[1]); // 关闭写端
        wait(0); // 等待子进程结束
    }
    exit(0);
}

void filter(int left_fd) {
    int prime;
    // 从左侧管道读取第一个整数，作为当前进程的素数
    if (read(left_fd, &prime, sizeof(prime)) == 0) {
        close(left_fd); // 关闭读取端
        exit(0); // 退出进程
    }

    // 打印当前进程的素数
    printf("prime %d\n", prime);

    int right_fd[2];
    // 创建到下一个子进程的管道
    pipe(right_fd);

    if (fork() == 0) {
        // 子进程
        close(right_fd[1]); // 关闭写端
        filter(right_fd[0]); // 调用过滤函数处理管道读取
    } else {
        // 父进程
        close(right_fd[0]); // 关闭读端
        int num;
        // 从左侧管道读取数据
        while (read(left_fd, &num, sizeof(num)) != 0) {
            // 如果该数不能被当前素数整除，则写入右侧管道
            if (num % prime != 0) {
                write(right_fd[1], &num, sizeof(num));
            }
        }
        close(left_fd); // 关闭左侧管道的读取端
        close(right_fd[1]); // 关闭右侧管道的写入端
        wait(0); // 等待子进程结束
        exit(0); // 退出进程
    }
}
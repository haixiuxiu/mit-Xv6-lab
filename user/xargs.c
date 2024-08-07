#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

// 读取标准输入的一行
char* readline() {
    char* buf = malloc(512);
    char* p = buf;
    while (read(0, p, 1) == 1) {
        if (*p == '\n' || *p == '\0') {
            *p = '\0';  // 将换行符或结束符替换为字符串结束符
            return buf;
        }
        p++;
    }
    if (p != buf) return buf;  // 返回非空行
    free(buf);
    return 0;  // 读完所有输入
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs <command>\n");
        exit(1);
    }

    char *argvs[MAXARG];
    int i;
    // 复制初始命令参数
    for (i = 1; i < argc; i++) {
        argvs[i - 1] = argv[i];
    }
    int base_argc = argc - 1;

    while (1) {
        char *p_line = readline();
        if (p_line == 0) {
            break;
        }

        // 将命令行参数加入到命令参数列表
        int nargc = base_argc;
        char *p = p_line;
        while (*p != '\0') {
            while (*p == ' ') p++;  // 跳过空格
            if (*p == '\0') break;  // 行末

            argvs[nargc++] = p;  // 参数开始
            while (*p != ' ' && *p != '\0') p++;  // 参数结束
            if (*p == ' ') {
                *p = '\0';  // 参数末尾加结束符
                p++;
            }
        }
        argvs[nargc] = 0;  // 参数列表以空指针结尾

        int pid = fork();
        if (pid == 0) {
            exec(argvs[0], argvs);  // 子进程执行命令
            fprintf(2, "exec %s failed\n", argvs[0]);
            exit(1);
        } else {
            wait(0);  // 父进程等待子进程结束
        }
        free(p_line);
    }
    exit(0);
}
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

// 递归查找指定名称的文件
void find(char *path, char *filename);

int main(int argc, char *argv[]) {
    // 检查命令行参数数量
    if (argc != 3) {
        fprintf(2, "Usage: find <directory> <filename>\n");
        exit(1);
    }

    // 调用 find 函数，开始查找
    find(argv[1], argv[2]);
    exit(0);
}

// find 函数：在目录树中查找指定名称的文件
void find(char *path, char *filename) {
    int fd;
    struct stat st;//文件状态结构体，用于获取文件类型。
    struct dirent de;//目录项结构体，用于读取目录内容。
    char buf[512], *p;//用于存储目录路径的缓冲区;指向缓冲区中的当前位置，用于构建路径。
    
    // 打开指定路径的文件或目录
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取文件状态信息
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
    case T_FILE:
        // 如果是文件，检查文件名是否匹配
        if (strcmp(path + strlen(path) - strlen(filename), filename) == 0) {
            printf("%s\n", path);  // 打印文件路径
        }
        break;

    case T_DIR:
        // 如果是目录，准备遍历目录内容
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);  // 复制当前路径到缓冲区
        p = buf + strlen(buf);
        *p++ = '/';
        
        // 读取目录内容
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0)  // 跳过无效的目录项
                continue;
            memmove(p, de.name, DIRSIZ);  // 将目录项名称复制到缓冲区
            p[DIRSIZ] = 0;  // 确保目录项名称以空字符结尾
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;  // 跳过 "." 和 ".." 目录
            // 递归调用 find 函数，处理子目录
            find(buf, filename);
        }
        break;
    }
    close(fd);  // 关闭文件描述符
}
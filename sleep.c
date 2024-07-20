#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc,char *argv[]){
    if(argc==1)
    {
        printf("ERROR!Please enter the sleep time");
    }
    else if(argc==2)
    {
        int time = atoi(argv[1]);
        if (time <= 0) {
            // 输出错误信息到标准错误
            printf("ERROR! Please input valid time\n");
        } 
        else {
            // 休眠指定的时间
            sleep(time);
            // 正常退出
            exit(0);
        }
    }
    else
    {
        printf("error: Please input no more than one time value\n");
    }
    exit(-1);
}
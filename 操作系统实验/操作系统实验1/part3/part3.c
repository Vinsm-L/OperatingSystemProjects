#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define PERM S_IRUSR|S_IWUSR 
#define MAX_SEQUENCE 8   //最大长度

int main()
{
    key_t shmid;  //the identifier for the shared memory segment
    char   *p_addr, *c_addr;   //pointer to the shared memory segment
    pid_t pid;    //进程id
    int share_data = 4096;
    int num;   //斐波那契数列的长度
    char* Fibonacci[6] = {"1, 1", "1, 1, 2", "1, 1, 2, 3", "1, 1, 2, 3, 5", "1, 1, 2, 3, 5, 8", "1, 1, 2, 3, 5, 8, 13"};
    scanf("%d", &num);   //从命令行接受参数
    if (num < MAX_SEQUENCE && num > 1) {    //保证参数不大于MAX_SEQUENCE
    	if ( (shmid = shmget(IPC_PRIVATE, share_data, S_IRUSR | S_IWUSR) ) == -1 ) {     //创建大小为share_data的共享内存段
        	fprintf(stderr, "Create Share Memory Error:%s\n\a", strerror(errno));
        	exit(1);
    	}
    } else {
    	exit(0);
    }
    
    pid = fork();   //创建子进程
    if (pid > 0) {
        p_addr = (char *) shmat(shmid, NULL, 0);
        wait(NULL);   //等待子进程结束
        printf("%s\n", p_addr);  //输出共享内存段中斐波那契数的值

        if ( shmdt(p_addr) == -1) {
			fprintf(stderr, "Unable to detach\n");
		}
        exit(0);
    } else if (pid == 0) {
        c_addr = (char *) shmat(shmid, NULL, 0);
        sprintf(c_addr, Fibonacci[num - 2]);    //把斐波那契数列的值写入共享内存
        exit(0);
    }
}
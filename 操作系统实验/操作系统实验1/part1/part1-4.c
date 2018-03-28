#include <stdio.h>  
#include <sys/types.h>  
#include <unistd.h>  
int main(void)  
{  
	int i, a = 0;
	pid_t pid[2];

    for (i = 0; i < 2; i++) {   //父进程与子进程在循环中涉及变量的值不尽相同，有些是从父进程复制后就没有改变的
		if (pid[i] = fork())    //（相对的）父进程将进入分支
			a = 1;

		printf("X");  
    }

    if (pid[0] == 0)        //根据pid[0]、pid[1]的值进行打印，结果4个进程pid[0]、pid[1]的值均不同
    	printf("%d\n", a);

    if (pid[1] == 0)
    	printf("%d\n", a);

    return 0;  
}
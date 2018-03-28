#include <stdio.h>  
#include <sys/types.h>  
#include <unistd.h>  
int main(void)  
{  
	int i, a = 0;
	pid_t pid;
	if(pid = fork())  //子进程不会进入该分支
		a = 1;

	for(i = 0; i < 2; i++) {  
		printf("X");  
	}

	if(pid == 0)   //子进程才能打印
		printf("%d\n", a);

	return 0;  
}
#include <stdio.h>  
#include <sys/types.h>  
#include <unistd.h> 
 
void main(){
   int pid1=fork();    //创建子进程
   printf("**1**\n");    //打印1
   int pid2=fork();    //创建子进程
   printf("**2**\n");    //打印2
   if(pid1==0){int pid3=fork();printf("**3**\n");}   //若当前进程是在父进程的 int pid1 = fork(); 这条语句中产生的，或从父进程继承的pid1值为0，则创建子进程
   else printf("**4**\n");

  // printf("%4d %4d\n",getppid(),getpid());  
}

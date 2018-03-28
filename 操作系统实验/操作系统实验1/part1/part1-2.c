#include <stdio.h>  
#include <sys/types.h>  
#include <unistd.h> 
 
void main(){
   int pid1=fork();    //创建第一个子进程
   int pid2=-1;        //把pid2初始化为-1，方便对子进程进行区分
   if (pid1 != 0) {    //这里只有父进程才能创建子进程
      pid2=fork();
   }
   if (pid1 > 0 && pid2 > 0) {      //父进程打印字符a
      printf("a\n");
   }
   if (pid1 == 0) {    //子进程1打印字符b
      printf("b\n");
   }
   if (pid2 == 0) {    //子进程2打印字符c
      printf("c\n");
   }
}

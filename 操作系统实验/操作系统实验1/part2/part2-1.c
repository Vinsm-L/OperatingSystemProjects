#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int wait_mark;

void waiting()
{
	while (wait_mark != 0);
}

void stop()
{
	wait_mark = 0;
}

main()
{
	int p1, p2;

	while((p1 = fork()) == -1);

	if(p1 > 0)
	{
		while((p2 = fork()) == -1);
		if(p2 > 0)
		{
		    wait_mark = 1;
		    signal(SIGINT, stop);  //设置收到信号ctrl C时执行stop函数
		    waiting();
		    kill(p1, 16);    //向进程p1发出信号16
		    kill(p2, 17);   //向进程p2发出信号16
		    wait(0);
		    wait(0);
		    printf("parent process is killed! \n");
		    exit(0);
		} else {
		    wait_mark = 1;
		    signal(SIGINT, SIG_IGN);	    
		    signal(17, stop); //设置收到信号17时执行stop函数
		    waiting();
		    printf("child process 2 is killed by parent! \n");
		    exit(0);
	    }
	} else {
	       wait_mark = 1;
           signal(SIGINT, SIG_IGN);
	       signal(16, stop); //设置收到信号16时执行stop函数
	       waiting();
	       printf("child process 1 is killed by parent! \n");
	       exit(0);
    }
}
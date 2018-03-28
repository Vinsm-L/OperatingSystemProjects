#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_LINE 80
#define BUFFER_SIZE 10

char buffer[10][80];    //存放命令的队列
int command_num = 0;    //队列中命令的数量
int isCheck = 0;        //标志捕获了Ctrl C
 /* 每次输入的命令规定不超过80个字符 */
/* * setup() 用于读入下一行输入的命令，并将它分成没有空格的命令和参数存于数组args[]中，
 * 用NULL作为数组结束的标志 
 */

void handle_SIGINT() {       //处理SIGINT信号，列出队列中的命令
	isCheck = 1;
	int i;
	printf("\n**list**\n");
	for (i = command_num - 1; i >= 0; i--) {     //倒序打印，时间最近的放最前面
		printf("%s\n", buffer[i]);
	}
}

/*
 * setup() 用于读入下一行输入的命令，并将它分成没有空格的命令和参数存于数组args[]中，
 * 用NULL作为数组结束的标志 
 */

void setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* 命令的字符数目 */
        i,      /* 循环变量 */
        start,  /* 命令的第一个字符位置 */
        ct;     /* 下一个参数存入args[]的位置 */
    
    ct = 0;
    int flag = 0;
    /* 读入命令行字符，存入inputBuffer */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
    if (isCheck) {                    //若捕获了Ctrl C，则需再读一次
    	isCheck = 0;
    	length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
    	flag = 1;
    }  

    start = -1;
    if (length == 0) exit(0);            /* 输入ctrl+d，结束shell程序 */
    if (length < 0) { 
        perror("error reading the command");
		exit(-1);           /* 出错时用错误码-1结束shell */
    }

    if (!flag) {
	    if (command_num > 9) {   //维护长度为10的队列
	       int i;
	       for (i = 1; i < 10; i++) {
	       	    strcpy(buffer[i-1], buffer[i]);    //队列满时队首出队
	       }
	       i = 0;
	       while (inputBuffer[i] != '\n') {     //队尾加入新命令
	    		buffer[9][i] = inputBuffer[i];
	    		i++;
	       }
	       buffer[9][i] = '\0';
	    } else {
	    	int i = 0;
	    	while (inputBuffer[i] != '\n') {             //给每条命令加上/0结束符，防止打印出错
	    		buffer[command_num][i] = inputBuffer[i];
	    		i++;
	    	}
	    	buffer[command_num][i] = '\0';
	    	command_num++;
	    }
    }
    

    /* 检查inputBuffer中的每一个字符 */
    for (i = 0; i < length; i++) {
        switch (inputBuffer[i]) {
	    case ' ':
	    case '\t' :               /* 字符为分割参数的空格或制表符(tab)'\t'*/
		if (start != -1) {
            args[ct] = &inputBuffer[start];    
		    ct++;
		}
        inputBuffer[i] = '\0'; /* 设置 C string 的结束符 */
		start = -1;
		break;

        case '\n':                 /* 命令行结束 */
		if (start != -1) {
            args[ct] = &inputBuffer[start];     
		    ct++;
		}
        inputBuffer[i] = '\0';
        args[ct] = NULL; /* 命令及参数结束 */
		break;

	    default :             /* 其他字符 */
		if (start == -1)
		    start = i;
        if (inputBuffer[i] == '&') {  
		    *background  = 1;          /*置命令在后台运行*/
            inputBuffer[i] = '\0';
		}
	    } 
    }    
    args[ct] = NULL; /* 命令字符数 > 80 */

    if (args[0][0] == 'r') {    
    	if (args[0][1] == '\0' && args[1] == NULL) {   //命令中只有r，执行最近的命令
    			int j = 0;
    			while (buffer[command_num-1][j] != '\0') {
    				inputBuffer[j] = buffer[command_num-1][j];
    				j++;
    			}
    			inputBuffer[j] = '\n';
    			//同setup上面的代码段的功能
			    int length, i, start, ct;
			    ct = 0;
    		    length = j+1;
    			start = -1;
				for (i = 0; i < length; i++) {
			      switch (inputBuffer[i]) {
			      case ' ':
			      case '\t': 
			        if (start != -1) {
			          args[ct] = &inputBuffer[start];
			          ct++;
			        }
			        inputBuffer[i] = '\0'; 
			        start = -1;
			        break;
			      case '\n':
			        if (start != -1) {
			          args[ct] = &inputBuffer[start];
			          ct++;
			        }
			        inputBuffer[i] = '\0';
			        args[ct] = NULL; 
			        break;
			      default: 
			        if (start == -1)
			          start = i;
			        if (inputBuffer[i] == '&') {
			          *background = 1; 
			          inputBuffer[i] = '\0';
			        }
			      }
   				args[ct] = NULL; /* 命令字符数 > 80 */
    		}
    	} else {   //命令中有r x，寻找x开头的命令
    		int isFound = -1;   //标志是否找到命令
    		int k;
    		for (k = command_num-1; k >= 0; k--) {
    			if (args[1][0] == buffer[k][0]) {
    				isFound = k;
    				break;
    			}
    		}
    		if (isFound != -1) {
    			int j = 0;
    			while (buffer[isFound][j] != '\0') {
    				inputBuffer[j] = buffer[isFound][j];
    				j++;
    			}
    			inputBuffer[j] = '\n';
    			//同setup上面代码段的功能
    			int length, i, start, ct;
			    ct = 0;
    		    length = j+1;
    			start = -1;
				for (i = 0; i < length; i++) {
			      switch (inputBuffer[i]) {
			      case ' ':
			      case '\t': 
			        if (start != -1) {
			          args[ct] = &inputBuffer[start];
			          ct++;
			        }
			        inputBuffer[i] = '\0'; 
			        start = -1;
			        break;
			      case '\n':
			        if (start != -1) {
			          args[ct] = &inputBuffer[start];
			          ct++;
			        }
			        inputBuffer[i] = '\0';
			        args[ct] = NULL; 
			        break;
			      default: 
			        if (start == -1)
			          start = i;
			        if (inputBuffer[i] == '&') {
			          *background = 1; 
			          inputBuffer[i] = '\0';
			        }
			      }
   				args[ct] = NULL; /* 命令字符数 > 80 */
    		}
    		}
    	}
    }

    //命令入队
    if (flag) {
	    if (command_num > 9) {   //维护长度为10的队列
	       int i;
	       for (i = 1; i < 10; i++) {
	       	    strcpy(buffer[i-1], buffer[i]);
	       }
	       i = 0;
	       while (inputBuffer[i] != '\0') {
	    		buffer[9][i] = inputBuffer[i];
	    		i++;
	       }
	       buffer[9][i] = '\0';
	    } else {
	    	int i = 0;
	    	while (inputBuffer[i] != '\0') {
	    		buffer[command_num][i] = inputBuffer[i];
	    		i++;
	    	}
	    	buffer[command_num][i] = '\0';
	    	command_num++;
	    }
    }
}

int main(void)
{
	struct sigaction handler;
	handler.sa_handler = handle_SIGINT;
	sigaction(SIGINT, &handler, NULL);

    char inputBuffer[MAX_LINE]; /* 这个缓存用来存放输入的命令*/
    int background;             /* ==1时，表示在后台运行命令，即在命令后加上'&' */
    char *args[MAX_LINE/2+1];/* 命令最多40个参数 */
    pid_t pid;

    while (1) {            /* 程序在setup中正常结束*/
		background = 0;
		printf("COMMAND->"); 
        fflush(stdout); //输出输出缓存内容
        setup(inputBuffer, args, &background);       /* 获取下一个输入的命令 */


		if ( (pid=fork()) == -1) {
			printf("Fork Error.\n");
		} // 创建失败

		if (pid == 0) {
			execvp(args[0], args); 
			exit(0);
		}

        if (background == 0) //父进程等待子进程退出
        	wait(0);
    }
}
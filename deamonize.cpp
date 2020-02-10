#include<stdio.h>
#include<syslog.h>
#include<fcntl.h>
#include<sys/resource.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/stat.h>
#include<fstream>
#include<string>

using namespace std;
//void writeLog1(string content);
void deamonize(const char* cmd);
/*int main(){
	deamonize("write file");
	FILE *fp = fopen("/home/molly/code/searchEngine/testd.txt","w+");
	for(int i =0;i<100;i++){
		putc('a',fp);
		sleep(1);		
	}
	fclose(fp);
}*/
void deamonize(const char* cmd){
	pid_t pid;
	int i,fd0,fd1,fd2;
	struct rlimit rl;
	struct sigaction sa;

	//调用umask屏蔽字修改文件的屏蔽字
	umask(0);
	//fork,杀掉父进程:新的进程不是组长
	if((pid = fork())<0){//fork出错
		printf("%s can not fork",cmd);
		return;
	}
	else if(pid!=0){//父进程
		exit(0);
	}
	//新建会话，设置会话id
	setsid();
	
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	if(sigaction(SIGHUP,&sa,NULL)<0){
		printf("%s:can't ignore SIGHUP",cmd);
		return;
	}

	if((pid=fork())<0){//fork出错
		printf("%s:can't fork",cmd);
		return;
	}
	else if(pid!=0)//父进程
		exit(0);

	//将当前的工作目录改到根目录
	//以防之前的工作目录被unmount
	if(chdir("/")<0){//改变工作路径失败
		printf("%s:can't change directory to /",cmd);
		exit(0);
	}

	//关闭之前的无用的文件描述符
//	if(rl.rlim_max == RLIM_INFINITY)//设置最大的文件描述符值?不等
	rl.rlim_max = 1024;
	for(i = 0;i<rl.rlim_max;i++)//遍历，关闭所有
		close(i);

	//把文件描述符的012链接到/dev/null上
	fd0 = open("/dev/null",O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	//初始化日志文件
	openlog(cmd,LOG_CONS,LOG_DAEMON);
	if(fd0 != 0 || fd1!=1 || fd2!=2 ){
		syslog(LOG_ERR,"unexpected file descriptors %d %d %d",fd0,fd1,fd2);
		exit(0);
	}
}

/*void writeLog1(string content){
	//输出到文件
	string filename = "/home/molly/code/searchEngine/searchEngine2.6/log/th_log.txt";
	ofstream outfile(filename.c_str(),ios::app);//创建输入流
    if(!outfile){
        return;
    }
	outfile << content<< endl;
	outfile.close();
}*/

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#define BUFSIZE 256
#define TIMEOUT 4


void sigmsg(){

printf("aa\n");

}

void myalarm(int sec) {
static int fd1[2],fd2[2];
int status,parent_id,pid,sig,pid2;
static int c_num,k_num,i,pid_mem,count;
   struct sigaction sa;
   sa.sa_handler = SIG_IGN;
   sa.sa_flags = SA_NOCLDWAIT;
   struct sigaction sa2;
   sa2.sa_handler = SIG_IGN;
   sa2.sa_flags = SA_NOCLDWAIT;
   parent_id=getpid();
       static double startTime;
	  static double  endTime;
          double endTime2,now,diff;
	    double totalTime = 0.0, setTime = sec;
      int p_write[2];
     int p_read[2];
      int p_wri[2];
      int p_rea[2];
      
if (pipe(fd1) == -1) {
perror("pipe failed.");
exit(1);
}

  
   if (sigaction(SIGCHLD, &sa, NULL) == -1) {
       exit(1);
   }


if(c_num==0){
if ((pid=fork())== -1) {
perror("fork failed.");
exit(1);
}
c_num++;
}
else{
i=0;
printf("term %d\n",pid_mem);
if (kill(pid_mem,SIGTERM) == -1) {
perror("kill failed.");
exit(1);
}

}



if (pid == 0) { /* Child process */
            
p_rea[0]=1;

while(i<1){
i++;
if ((pid2=fork())== -1) {
perror("fork failed.");
exit(1);
}



if(pid2==0){//child2
sleep(TIMEOUT);
exit(0);
}
else{  //parent2
pid_mem=pid2;
printf("create %d\n",pid_mem);
                 p_write[0]=pid_mem;
                close(fd1[0]);        
	     if (write(fd1[1], p_write,sizeof(p_write)) == -1) {
		perror("child write.");
		exit(1);
             }

wait(&status);

}

}

                  if (kill(parent_id,SIGALRM) == -1) {
                    perror("kill failed2.");
                     exit(1);
                    }
  



}
else {

	close(fd1[1]);
	if (read(fd1[0], p_read, sizeof(p_read)+10) == -1) {
	perror("pipe read.");
	exit(1);
	} 

     pid_mem=p_read[0];

 

}




}


void timeout()
{
printf("This program is timeout.\n");
exit(0);
}
int main()
{
char buf[BUFSIZE];
if(signal(SIGALRM,timeout) == SIG_ERR) {
perror("signal failed.");
exit(1);
}
myalarm(TIMEOUT);
while (fgets(buf, BUFSIZE, stdin) != NULL) {
printf("echo: %s",buf);
myalarm(TIMEOUT);
}
}

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
int status,parent_id,pid,sig;
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
       double p_write[2];
       double p_read[2];
      int p_wr[2];
      int p_re[2];
      

if (pipe(fd1) == -1) {
perror("pipe failed.");
exit(1);
}

if (pipe(fd2) == -1) {
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
//
}
else{

i=0;
printf("%d\n",i);

}



if (pid == 0) { /* Child process */
            
	   /* while(1){
             

	        if(totalTime > setTime) {
                  if (kill(parent_id,SIGALRM) == -1) {
                    perror("kill failed.");
                     exit(1);
                    }
    
                   break;
                 }

	        endTime = clock() / CLOCKS_PER_SEC ;


	        totalTime = endTime - startTime;
                p_write[0]=endTime;
                close(fd1[0]);        
	     if (write(fd1[1], p_write,sizeof(p_write)) == -1) {
		perror("child write.");
		exit(1);
             }
           

	    }*/

while(i<TIMEOUT){
sleep(1);

i=i+1;

}

                  if (kill(parent_id,SIGALRM) == -1) {
                    perror("kill failed.");
                     exit(1);
                    }


exit(0);
}
else {
	/*close(fd1[1]);
	if (read(fd1[0], p_read, sizeof(p_read)+10) == -1) {
	perror("pipe read.");
	exit(1);
	} */
     // startTime=p_read[0];

pid_mem=pid;

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

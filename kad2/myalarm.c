#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFSIZE 256
#define TIMEOUT 10




void myalarm(int sec) {

int pid,parent_id;
static int c_num,pid_mem;
   struct sigaction sa;
   sa.sa_handler = SIG_IGN;
   sa.sa_flags = SA_NOCLDWAIT;

   parent_id=getpid();
  
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
if (kill(pid_mem,SIGTERM) == -1) {
perror("kill failed.");
exit(1);
}

if ((pid=fork())== -1) {
perror("fork failed.");
exit(1);
}
c_num++;

}



if (pid == 0) { /* Child process */
             
  sleep(TIMEOUT);

                  if (kill(parent_id,SIGALRM) == -1) {
                    perror("kill failed.");
                     exit(1);
                    }

exit(0);
}
else {   /* parent process */
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

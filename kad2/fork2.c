#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define NUMPROCS 3
static int num;

void main()
{
int i, pid, status, c_pid;

if ((pid=fork())== -1) {
perror("fork failed.");
exit(1);
}
if (pid == 0) { /* Child process */
num=100;
exit(0);
}

else{
wait(&status);
printf("%d\n",num);
}

}

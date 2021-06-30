#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/types.h>

int fd[2];
int pid,status;

void fork1(int n){
int k=n-1;
if(k>0){
if ((pid=fork())== -1) {
perror("fork failed.");
exit(1);
}
if(pid==0){
int pro_id3=getpid();
printf("child:%d / id=%d\n",k,pro_id3);
}
else{
int pro_id=getpid();
printf("parent:%d / id=%d\n",k,pro_id);
//wait(&status);
fork1(k);
wait(&status);
}
}
}

void main(){
fork1(4);
int pro_id2=getpid();
printf("%d\n",pro_id2);

}

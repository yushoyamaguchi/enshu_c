#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#define NUMPROCS 4

int i2, sid,semval;
key_t key;
struct sembuf sb;






int main(){
int i,pid1,status1,pid2,status2,k,one_pid,two_pid;
int fd1[2];
int fd2[2];
int read1[2],read2[2];
int write1[2],write2[2];



if (pipe(fd1) == -1) {
perror("pipe failed.");
exit(1);
}
if (pipe(fd2) == -1) {
perror("pipe failed.");
exit(1);
}




if ((pid1=fork())== -1) {
perror("fork failed.");
exit(1);
}
if (pid1 == 0) { /* Child process1 */
 int a[10];
 one_pid=getpid();
   for(k=0;k<10;k++){
      a[k]=k;
     }     

//close(fd2[1]);
if (read(fd2[0], read2, sizeof(read2)+1) == -1) {
perror("child read.");
exit(1);
}     
two_pid=read2[0];     

   for(k=0;k<10;k++){
     
       printf("%d:process1\n",a[k]);
     // semval_down();
      
          
       if (kill(two_pid,SIGCONT) == -1) {
                perror("kill failed.");
                exit(1);
        }
     

     if(k<9) {
       if (kill(one_pid,SIGSTOP) == -1) {
                perror("kill failed.");
                exit(1);
        }
       }
      //semval_up();
     }          

  usleep(100);  

exit(0);
}
else{
one_pid=pid1;  
write1[0]=one_pid;
//close(fd1[0]);
if (write(fd1[1], write1, sizeof(write1)) == -1) {
perror("parent write.");
exit(1);
}
}



if ((pid2=fork())== -1) {
perror("fork failed.");
exit(1);
}
if (pid2 == 0) { /* Child process1 */
 int a[10];
 two_pid=getpid();
   for(k=0;k<10;k++){
      a[k]=k;
     }
     
//close(fd1[1]);
if (read(fd1[0], read1, sizeof(read1)+1) == -1) {
perror("child read.");
exit(1);
}     
one_pid=read1[0];


   for(k=0;k<10;k++){
     
       printf("%d:process2\n",a[k]);
      //semval_down();
      

       if (kill(one_pid,SIGCONT) == -1) {
                perror("kill failed.");
                exit(1);
        }
      

 if(k<9) {
       if (kill(two_pid,SIGSTOP) == -1) {
                perror("kill failed.");
                exit(1);
        }
  }
     // semval_up();
     }          

    
usleep(100); 
exit(0);
}
else{
two_pid=pid2; 
write2[0]=two_pid;
//close(fd2[0]);
if (write(fd2[1], write2, sizeof(write2)) == -1) {
perror("parent write.");
exit(1);
} 

}



wait(&status1);
wait(&status2);


exit(0);
}





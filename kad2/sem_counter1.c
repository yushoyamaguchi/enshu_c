#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define NUMPROCS 4
char filename[]="counter";
int i2, sid,semval;
key_t key;
struct sembuf sb;


void semval_up(){
sb.sem_num = 0;
sb.sem_op = 1;
sb.sem_flg = 0;
if (semop(sid, &sb, 1) == -1) {
perror("sem_signal semop error.");
exit(1);
}
/*semval = semctl( sid, 0, GETVAL, 0 );
printf("semval = %d\n",semval);*/

}

void semval_down(){
sb.sem_num = 0;
sb.sem_op = -1;
sb.sem_flg = 0;
if (semop(sid, &sb, 1) == -1) {
perror("sem_wait semop error.");
exit(1);
}

/*semval = semctl( sid, 0, GETVAL, 0 );
printf("%d\n",semval);*/

}

int count1()
{
FILE *ct;
int count;
semval_down();
if ((ct=fopen(filename, "r"))==NULL) exit(1);
fscanf(ct, "%d\n", &count);
count++;
fclose(ct);
if ((ct=fopen(filename, "w"))==NULL) exit(1);
fprintf(ct, "%d\n", count);
fclose(ct);
semval_up();
return count;
}



int main()
{

setbuf(stdout, NULL); /* set stdout to be unbufferd */
if ((key = ftok(".", 1)) == -1){
fprintf(stderr,"ftok path does not exist.\n");
exit(1);
}

if ((sid=semget(key, 1, 0666 | IPC_CREAT)) == -1) {
perror("semget error.");
exit(1);
}

while((semval = semctl( sid, 0, GETVAL, 0 ))!=1){

if(semval<1) semval_up();
else if(semval>1) semval_down();


}



int i,ii, count, pid, status;
FILE *ct;
setbuf(stdout, NULL); /* set stdout to be unbufferd */
count = 0;
if ((ct=fopen(filename, "w"))==NULL) exit(1);
fprintf(ct, "%d\n", count);
fclose(ct);
for (i=0; i<NUMPROCS; i++) {
if ((pid=fork())== -1) {
perror("fork failed.");
exit(1);
}
if (pid == 0) { /* Child process */

count = count1();
printf("count = %d  \n", count);
exit(0);

}
}


for (ii=0; ii<NUMPROCS; ii++) {
wait(&status);
}

exit(0);
}

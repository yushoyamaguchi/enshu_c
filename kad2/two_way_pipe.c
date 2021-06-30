#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define BUFSIZE 256
int main(int argc, char *argv[])
{
char buf[BUFSIZE];
char buf2[BUFSIZE];
int fd[2];
int fd2[2];
int pid, msglen, status ,msglen2;
if (argc != 3) {
printf("bad argument.\n");
exit(1);
}
if (pipe(fd) == -1) {
perror("pipe failed.");
exit(1);
}
if (pipe(fd2) == -1) {
perror("pipe failed.");
exit(1);
}
if ((pid=fork())== -1) {
perror("fork failed.");
exit(1);
}
if (pid == 0) { /* Child process */
//close(fd[0]);
msglen = strlen(argv[1]) + 1;
if (write(fd2[1], argv[1], msglen) == -1) {
perror("child write.");
exit(1);
}
if (read(fd[0], buf, BUFSIZE) == -1) {
perror("child read.");
exit(1);
}
printf("Message from patent process: \n");
printf("\t%s\n",buf);

exit(0);

} else { /* Parent process */
//close(fd[1]);
msglen2 = strlen(argv[2]) + 1;
if (write(fd[1], argv[2], msglen2) == -1) {
perror("parent write.");
exit(1);
}

if (read(fd2[0], buf2, BUFSIZE) == -1) {
perror("parent read.");
exit(1);
}

printf("Message from child process: \n");
printf("\t%s\n",buf2);
wait(&status);
}
}

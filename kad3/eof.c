#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <sys/un.h>

int main(){
char a[32]="hogehoge";
char b[5][32];


strcpy(b[0],a);
printf("%s\n",b[0]);



}

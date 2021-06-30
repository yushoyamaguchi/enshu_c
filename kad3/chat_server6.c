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
#include <signal.h>
#include <ctype.h>

#define MAXCLIENTS 5

int sock;
int csock[6];
int avai_min,maxfd;
int k=0;
struct sockaddr_in svr;
struct sockaddr_in clt;
struct hostent *cp;
int clen,nbytes,reuse;
int i,k;
char rbuf[1024];
char msg[1024];
fd_set rfds;
static int check[6];
char username[5][1024];
char username2[1024];
int un_check=0;
int len;
int tt=0;
char *token,*token2;
char rbuf2[1024];
char secret[3][1024];

void recieve (int n){

if(check[n]==1){
 memset(rbuf, 0, sizeof(rbuf));
read(csock[n],rbuf,sizeof(rbuf));
strcpy(rbuf2,rbuf);
int p=0;
int pp;


for(i=0;i<3;i++){
pp=0;
while((isspace(rbuf2[p]))==0){
secret[i][pp]=rbuf2[p];

p++;
pp++;
}
secret[i][pp]='\0';
p++;

}



if(strcmp(rbuf,"")==0){
close(csock[n]);
printf("left from chat : %s",username[n]);
tt=1;
strcpy(username[n],"");
k--;
check[n]=0;

}
else if(strcmp(secret[0],"/send")==0){


k=0;
strcat(secret[1],"\n");

for(i=0;i<5;i++){
if(strcmp(secret[1],username[i])==0){
k=1;
break;
}
}
if(k==1){

memset(msg, 0, sizeof(msg));

strcpy(msg,username[n]);
len=strlen(msg);
msg[len-1]='\0';
strcat(msg," > ");
strcat(msg," {private} ");
len=strlen(secret[2]);
secret[2][len]='\n';
strcat(msg,secret[2]);

write(csock[i],msg,sizeof(msg));
}

}
else{
for(i=0;i<5;i++){
if(check[i]==1 ){
memset(msg, 0, sizeof(msg));


strcpy(msg,username[n]);
len=strlen(msg);
msg[len-1]='\0';
strcat(msg," > ");
strcat(msg,rbuf);

write(csock[i],msg,sizeof(msg));
}
}
}
}



}


void sigmsg(){
int pid;
pid=getpid();
for(i=0;i<5;i++){
if(check[i]==1){
char con_msg2[] = "This program will end in 3 seconds.\n";
write(csock[i],con_msg2,sizeof(con_msg2));
}
}
sleep(3);

if (kill(pid,SIGTERM) == -1) {
perror("kill failed.");
exit(1);
}

}





int main(int argc,char **argv) {



if(signal(SIGINT,sigmsg) == SIG_ERR) {
perror("signal failed.");
exit(1);
}


/* select() で用いるファイル記述子集合 */
struct timeval tv;
/* select() が返ってくるまでの待ち時間を指定する変数 */


/* ソケットの生成 */
if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {
perror("socket");
exit(1);
}
/* ソケットアドレス再利用の指定 */
reuse=1;

if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0) {
perror("setsockopt");
exit(1);
}
/* client 受付用ソケットの情報設定 */
bzero(&svr,sizeof(svr));
svr.sin_family=AF_INET;
svr.sin_addr.s_addr=htonl(INADDR_ANY); /* 受付側の IP アドレスは任意 */
svr.sin_port=htons(10140); /* ポート番号 10140 を介して受け付ける */
/* ソケットにソケットアドレスを割り当てる */
if(bind(sock,(struct sockaddr *)&svr,sizeof(svr))<0) {
perror("bind");
exit(1);
}
maxfd=sock;
/* 待ち受けクライアント数の設定 */
if (listen(sock,5)<0) { /* 待ち受け数に 5 を指定 */
perror("listen");
exit(1);
}





do{

//printf("aa\n");

/* 入力を監視するファイル記述子の集合を変数 rfds にセットする */
FD_ZERO(&rfds);
/* rfds を空集合に初期化 */
FD_SET(0,&rfds);
/* 標準入力 */
FD_SET(sock,&rfds); /* クライアントを受け付けたソケット */
for(i=0;i<5;i++){
if(check[i]==1){
FD_SET(csock[i],&rfds);
}
} 
/* 監視する待ち時間を 1 秒に設定 */
tv.tv_sec = 1;
tv.tv_usec = 0;
/* 標準入力とソケットからの受信を同時に監視する */

if(select(maxfd+1,&rfds,NULL,NULL,&tv)>0) {

if(FD_ISSET(sock,&rfds)) { /* ソケットから受信したなら */
/* ソケットから読み込み端末に出力 */




for(i=0;i<MAXCLIENTS+1;i++){
if(check[i]==0){
avai_min=i;
check[i]=1;

break;
}
}

/* クライアントの受付 */
clen = sizeof(clt);
if ( ( csock[avai_min] = accept(sock,(struct sockaddr *)&clt,&clen) ) <0 ) {
perror("accept");
exit(2);
}

if (csock[avai_min] > maxfd) {
   maxfd = csock[avai_min];
 } 


if(k<MAXCLIENTS && check[5]==0){



if (csock[avai_min] > maxfd) {
   maxfd = csock[avai_min];
 } 




cp = gethostbyaddr((char *)&clt.sin_addr,sizeof(struct in_addr),AF_INET);

char con_msg[17] = "REQUEST ACCEPTED\n";
write(csock[avai_min],con_msg,sizeof(con_msg));

memset(rbuf, 0, sizeof(rbuf));
read(csock[avai_min],rbuf,sizeof(rbuf));


for(i=0;i<5;i++){
if(i!=avai_min && check[i]==1){
if((strcmp(rbuf,username[i]))==0){
char con_msg3[] = "USERNAME REJECTED\n";
write(csock[avai_min],con_msg3,sizeof(con_msg3));
close(csock[avai_min]);
un_check=1;
}
}
}



if(un_check==0){

strcpy(username[avai_min],rbuf);
 
char con_msg4[20] = "USERNAME REGISTERED\n";

write(csock[avai_min],con_msg4,sizeof(con_msg4));
char con_msg5[] = "the username of the new member  is ";
strcat(con_msg5,username[avai_min]);
printf("%s\n",con_msg5);
k++;
}
else if(un_check==1){
un_check=0;
printf("%s have already exist\n",rbuf);
close(csock[avai_min]);
check[avai_min]=0;
}



}

else{
char con_msg2[] = "REQUEST REJECTED\n";
write(csock[avai_min],con_msg2,sizeof(con_msg2));
printf("too many clients\n");
close(csock[5]);
check[5]=0;
}


}



if(FD_ISSET(csock[0],&rfds)) {

recieve(0);
}

if(FD_ISSET(csock[1],&rfds)) {

recieve(1);
}

if(FD_ISSET(csock[2],&rfds)) {

recieve(2);
}

if(FD_ISSET(csock[3],&rfds)) {

recieve(3);
}

if(FD_ISSET(csock[4],&rfds)) {


recieve(4);

}

}


} while(1);



}

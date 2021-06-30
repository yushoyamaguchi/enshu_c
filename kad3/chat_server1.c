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

#define MAXCLIENTS 5

int main(int argc,char **argv) {
int sock;
int csock[5];
int avai_min,maxfd;
int k=0;
struct sockaddr_in svr;
struct sockaddr_in clt;
struct hostent *cp;
int clen,nbytes,reuse;
int i;
char rbuf[1024];
char msg[1024];
fd_set rfds;
static int check[5];
char username[5][1024];
char username2[1024];
int un_check=0;
int len;
int tt=0;

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

if(tt==1) printf("aa\n");

/* 入力を監視するファイル記述子の集合を変数 rfds にセットする */
FD_ZERO(&rfds);
/* rfds を空集合に初期化 */
FD_SET(0,&rfds);
/* 標準入力 */
FD_SET(sock,&rfds); /* クライアントを受け付けたソケット */
for(i=0;i<4;i++){
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


if(k<MAXCLIENTS-1){

for(i=0;i<MAXCLIENTS-1;i++){
if(check[i]==0){
avai_min=i;
check[i]=1;
printf("check[%d]=1\n",i);
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

FD_SET(csock[avai_min],&rfds); /* クライアントを受け付けたソケット */
 /*memset(rbuf, 0, sizeof(rbuf));
nbytes = read(csock[avai_min],rbuf,sizeof(rbuf));*/

/* クライアントのホスト情報の取得 */
cp = gethostbyaddr((char *)&clt.sin_addr,sizeof(struct in_addr),AF_INET);
//printf("connected\n");
printf("[%s]\n",cp->h_name);

char con_msg[] = "REQUEST ACCEPTED\n";
write(csock[avai_min],con_msg,sizeof(con_msg));

memset(rbuf, 0, sizeof(rbuf));
read(csock[avai_min],rbuf,sizeof(rbuf));


for(i=0;i<5;i++){
if(i!=avai_min && check[i]==1){
if((strcmp(rbuf,username[avai_min]))==0){
char con_msg3[] = "USERNAME REJECTED\n";
write(csock[avai_min],con_msg3,sizeof(con_msg3));
close(csock[avai_min]);
un_check=1;
}
}
}

if(un_check==0){

strcpy(username[avai_min],rbuf);
 
char con_msg4[] = "USERNAME REGISTERED\n";
//printf("%s\n",con_msg4);
write(csock[avai_min],con_msg4,sizeof(con_msg4)+1);
char con_msg5[] = "the username of the new member  is ";
strcat(con_msg5,username[avai_min]);
printf("%s\n",con_msg5);
k++;
}
else if(un_check==1){
un_check=0;
}



}

else{
char con_msg2[] = "REQUEST REJECTED\n";
write(csock[avai_min],con_msg2,sizeof(con_msg2));
close(csock[avai_min]);
check[avai_min]=0;
}


}



if(FD_ISSET(csock[0],&rfds)) {
printf("srart0\n");
if(check[0]==1){
 memset(rbuf, 0, sizeof(rbuf));
read(csock[0],rbuf,sizeof(rbuf));
printf("%s:%s",username[0],rbuf);
if(strcmp(rbuf,"")==0){
close(csock[0]);
printf("left from chat : %s",username[0]);
tt=1;
strcpy(username[0],"");
k--;
check[0]=0;
//exit(0);
}
else{
for(i=0;i<5;i++){
if(check[i]==1  ){
strcpy(msg,username[0]);
len=strlen(msg);
msg[len-1]='\0';
strcat(msg," > ");
strcat(msg,rbuf);
write(csock[i],msg,sizeof(msg));

}
}
}
}
printf("end0\n");
}

if(FD_ISSET(csock[1],&rfds)) {
printf("srart1\n");
if(check[1]==1){
 memset(rbuf, 0, sizeof(rbuf));
read(csock[1],rbuf,sizeof(rbuf));
printf("%s:%s",username[1],rbuf);
if(strcmp(rbuf,"")==0){
close(csock[1]);
printf("left from chat : %s",username[1]);
strcpy(username[1],"");
k--;
check[1]=0;
//exit(0);
}
else{
for(i=0;i<5;i++){
if(check[i]==1  ){
strcpy(msg,username[1]);
len=strlen(msg);
msg[len-1]='\0';
strcat(msg," > ");
strcat(msg,rbuf);

write(csock[i],msg,sizeof(msg));
}
}
}
}
printf("end1\n");
}

if(FD_ISSET(csock[2],&rfds)) {
if(check[2]==1){
 memset(rbuf, 0, sizeof(rbuf));
read(csock[2],rbuf,sizeof(rbuf));
if(strcmp(rbuf,"")==0){
close(csock[2]);
printf("left from chat : %s",username[2]);
strcpy(username[2],"");
k--;
check[2]=0;
//exit(0);
}
else{
for(i=0;i<5;i++){
if(check[i]==1 && i!=2){
strcpy(msg,username[2]);
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

if(FD_ISSET(csock[3],&rfds)) {
if(check[3]==1){
 memset(rbuf, 0, sizeof(rbuf));
read(csock[3],rbuf,sizeof(rbuf));
if(strcmp(rbuf,"")==0){
close(csock[3]);
printf("left from chat : %s",username[3]);
strcpy(username[3],"");
k--;
check[3]=0;
//exit(0);
}
else{
for(i=0;i<5;i++){
if(check[i]==1 && i!=3){
strcpy(msg,username[3]);
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

if(FD_ISSET(csock[4],&rfds)) {
if(check[4]==1){
 memset(rbuf, 0, sizeof(rbuf));
read(csock[4],rbuf,sizeof(rbuf));
if(strcmp(rbuf,"")==0){
close(csock[4]);
printf("left from chat : %s",username[4]);
strcpy(username[4],"");
k--;
check[4]=0;
//exit(0);
}
else{
for(i=0;i<5;i++){
if(check[i]==1 && i!=4){
strcpy(msg,username[4]);
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



}


} while(1);



}

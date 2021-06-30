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
int main(int argc,char **argv) {
int sock;
struct sockaddr_in clt;
struct hostent *hp;
int clen,nbytes;
int len;
char buf[1024];
char inp[1024];
int buf_len;
int csock;
fd_set rfds;
/* select() で用いるファイル記述子集合 */
struct timeval tv;
/* select() が返ってくるまでの待ち時間を指定する変数 */

  if (argc != 2) {
    fprintf(stderr, "usage: %s machine-name\n", argv[0]);
    exit(1);
  }

  if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("client : socket");
    exit(1);
  }

bzero(&clt,sizeof(clt));
clt.sin_family=AF_INET;

if ( ( hp = gethostbyname(argv[1]) ) == NULL ) {
fprintf(stderr,"unknown host %s\n",argv[1]);
exit(1);
}

bcopy(hp->h_addr,&clt.sin_addr,hp->h_length);

clt.sin_port=htons(10130);

  if (connect(sock, (struct sockaddr *)&clt,sizeof(clt))  ==-1) {
    perror("client : connect");
  
    close(sock);
    exit(1);
  }
else printf("connected\n");
  

clen = sizeof(clt);
csock = accept(sock,(struct sockaddr *)&clt,&clen);

do{
/* 入力を監視するファイル記述子の集合を変数 rfds にセットする */
FD_ZERO(&rfds);
/* rfds を空集合に初期化 */
FD_SET(0,&rfds);
/* 標準入力 */
FD_SET(sock,&rfds); /* クライアントを受け付けたソケット */
/* 監視する待ち時間を 1 秒に設定 */
tv.tv_sec = 1;
tv.tv_usec = 0;
/* 標準入力とソケットからの受信を同時に監視する */
if(select(sock+1,&rfds,NULL,NULL,&tv)>0) {
if(FD_ISSET(0,&rfds)) { /* 標準入力から入力があったなら */
/* 標準入力から読み込みクライアントに送信 */
memset(buf, 0, sizeof(buf));

if(fgets(inp, sizeof(inp), stdin)!=NULL){
len=strlen(inp)+2;
write(sock,inp,len);
memset(inp, 0, sizeof(inp));
}
else {
 close(sock);
 exit(0);
 return 0;

}


}
if(FD_ISSET(sock,&rfds)) { /* ソケットから受信したなら */
/* ソケットから読み込み端末に出力 */
 memset(buf, 0, sizeof(buf));
nbytes = read(sock,buf,sizeof(buf));
if(strcmp(buf,"")==0){
close(sock);
exit(0);
printf("closed\n");

}
//printf("aaa\n");
printf("recieve : %s",buf);
//exit(0);
}
}
}while(1);

 close(sock);
 return 0;

}

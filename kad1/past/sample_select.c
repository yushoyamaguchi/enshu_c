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

int csock;
/* クライアントを受け付けたソケット */
fd_set rfds;
/* select() で用いるファイル記述子集合 */
struct timeval tv;
/* select() が返ってくるまでの待ち時間を指定する変数 */
...
/* クライアントからの接続待ち受けなど */
...
do{
/* 入力を監視するファイル記述子の集合を変数 rfds にセットする */
FD_ZERO(&rfds);
/* rfds を空集合に初期化 */
FD_SET(0,&rfds);
/* 標準入力 */
FD_SET(csock,&rfds); /* クライアントを受け付けたソケット */
/* 監視する待ち時間を 1 秒に設定 */
tv.tv_sec = 1;
tv.tv_usec = 0;
/* 標準入力とソケットからの受信を同時に監視する */
if(select(csock+1,&rfds,NULL,NULL,&tv)>0) {
if(FD_ISSET(0,&rfds)) { /* 標準入力から入力があったなら */
/* 標準入力から読み込みクライアントに送信 */
...
}
if(FD_ISSET(csock,&rfds)) { /* ソケットから受信したなら */
/* ソケットから読み込み端末に出力 */
...
}
}
} while(1); /* 繰り返す */

}

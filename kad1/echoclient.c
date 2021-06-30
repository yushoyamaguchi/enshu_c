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
int len;
char buf[32];
char inp[32];
int buf_len;

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

clt.sin_port=htons(10120);


  if (connect(sock, (struct sockaddr *)&clt,sizeof(clt))  ==-1) {
    perror("client : connect");
  
    close(sock);
    exit(1);
  }


while (fgets(inp, sizeof(inp), stdin)!= NULL){

len=strlen(inp)+2;

write(sock,inp,len);

 memset(buf, 0, sizeof(buf));
 buf_len = read(sock, buf, sizeof(buf));

 printf("%s",  buf);
 memset(inp, 0, sizeof(inp));

}



 close(sock);
 return 0;

}


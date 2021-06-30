#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>

void count(int num){
static int count;
count++;
printf("%d:%d回目\n",count,num);
sleep(4);

}

main()
{
char buf[127];
for(int i=0;i<3;i++){
count(i);
 
}

}

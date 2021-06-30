/*----------------------------------------------------------------------------
 *  簡易版シェル
 *--------------------------------------------------------------------------*/

/*
 *  インクルードファイル
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>

/*
 *  定数の定義
 */

#define BUFLEN    1024     /* コマンド用のバッファの大きさ */
#define MAXARGNUM  256     /* 最大の引数の数 */

typedef struct dir_list{
  char            *name;
  struct dir_list *prev;
  struct dir_list *next;
} DIR_LIST;

typedef struct dir_list2{
  char        *com_name;
  char        *new_name;   
  struct dir_list2 *prev;
  struct dir_list2 *next;
} DIR_LIST2;



DIR_LIST *root;
DIR_LIST *old_com;
DIR_LIST *latest_com;
DIR_LIST2 *latest_alias;
char *prompt;


/*
 *  ローカルプロトタイプ宣言
 */

int parse(char [], char *[]);
void execute_command(char *[], int);
void execute_command2(char *[], int);
void change_dir();
void test_com();
void pushd_com( void );
void history_inp( char * );
void history_com( void );
void history_search( char *st );
void history_search2( char *st );
void dirs_com( void );
void history_main(char *command_buffer);
DIR_LIST *new_item(char *name,  DIR_LIST *post_item, DIR_LIST *pre_item);
DIR_LIST2 *new_item2(char *com_name,char *new_name , DIR_LIST2 *post_item, DIR_LIST2 *pre_item);
void clear_list(DIR_LIST *item);
void clear_list2(DIR_LIST2 *item);
void popd_com(void);
void script(char *);
void alias_com(char * , char *);
char *alias_check(char *);
void unalias_com(char *);
int wild_card_check(char *[]);
void wild_card(char *[], int, int);
char *prompt_com(char *);
void self_cat(char *);

/*----------------------------------------------------------------------------
 *
 *  関数名   : main
 *
 *  作業内容 : シェルのプロンプトを実現する
 *
 *  引数     :
 *
 *  返り値   :
 *
 *  注意     :
 *
 *--------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    char command_buffer[BUFLEN]; /* コマンド用のバッファ */
    char *args[MAXARGNUM];       /* 引数へのポインタの配列 */
    int command_status;         
     char *k;
    int n=-1;
  prompt="Command";

                                         /* コマンドの状態を表す
                                   command_status = 0 : フォアグラウンドで実行
                                    command_status = 1 : バックグラウンドで実行
                                    command_status = 2 : シェルの終了
                                    command_status = 3 : 何もしない */

    /*
     *  無限にループする
     */
       printf("%s : ",prompt);
   while((fgets(command_buffer, BUFLEN, stdin)) != NULL) {

        /*
         *  プロンプトを表示する
         */

        //printf("Command : ");

        /*
         *  標準入力から１行を command_buffer へ読み込む
         *  入力が何もなければ改行を出力してプロンプト表示へ戻る
         */

     /*   if((k=fgets(command_buffer, BUFLEN, stdin)) == NULL) {
            printf("\n");
            continue;
        }*/



        /*
         *  入力されたバッファ内のコマンドを解析する
         *
         *  返り値はコマンドの状態
         */

         history_inp(command_buffer);   //ヒストリーに加える(自分で追記しました)

        command_status = parse(command_buffer, args);

        /*
         *  終了コマンドならばプログラムを終了
         *  引数が何もなければプロンプト表示へ戻る
         */

        if(command_status == 2) {
            printf("done.\n");
            clear_list(root);
            clear_list(old_com);
            //clear_list2(latest_alias);
            exit(EXIT_SUCCESS);
        } else if(command_status == 3) {
            continue;
        }

        /*
         *  コマンドを実行する
         */
    
         args[0]=alias_check(args[0]);

         n=wild_card_check(args); 
          //printf("%d\n",n);
        if(n!=-1){
            //printf("aaa\n");
           wild_card(args,n,command_status);
           }
         
        else {
            execute_command(args, command_status);
          }
         printf("%s : ",prompt);
    }

    return 0;
}

/*----------------------------------------------------------------------------
 *
 *  関数名   : parse
 *
 *  作業内容 : バッファ内のコマンドと引数を解析する
 *
 *  引数     :
 *
 *  返り値   : コマンドの状態を表す :
 *                0 : フォアグラウンドで実行
 *                1 : バックグラウンドで実行
 *                2 : シェルの終了
 *                3 : 何もしない
 *
 *  注意     :
 *
 *--------------------------------------------------------------------------*/

int parse(char buffer[],        /* バッファ */
          char *args[])         /* 引数へのポインタ配列 */
{
    int arg_index;   /* 引数用のインデックス */
    int status;   /* コマンドの状態を表す */

    /*
     *  変数の初期化
     */

    arg_index = 0;
    status = 0;

    /*
     *  バッファ内の最後にある改行をヌル文字へ変更
     */

    *(buffer + (strlen(buffer) - 1)) = '\0';

    /*
     *  バッファが終了を表すコマンド（"exit"）ならば
     *  コマンドの状態を表す返り値を 2 に設定してリターンする
     */

    if(strcmp(buffer, "exit") == 0) {

        status = 2;
        return status;
    }

    /*
     *  バッファ内の文字がなくなるまで繰り返す
     *  （ヌル文字が出てくるまで繰り返す）
     */

    while(*buffer != '\0') {

        /*
         *  空白類（空白とタブ）をヌル文字に置き換える
         *  これによってバッファ内の各引数が分割される
         */

        while(*buffer == ' ' || *buffer == '\t') {
            *(buffer++) = '\0';
        }

        /*
         * 空白の後が終端文字であればループを抜ける
         */

        if(*buffer == '\0') {
            break;
        }

        /*
         *  空白部分は読み飛ばされたはず
         *  buffer は現在は arg_index + 1 個めの引数の先頭を指している
         *
         *  引数の先頭へのポインタを引数へのポインタ配列に格納する
         */

        args[arg_index] = buffer;
        ++arg_index;

        /*
         *  引数部分を読み飛ばす
         *  （ヌル文字でも空白類でもない場合に読み進める）
         */

        while((*buffer != '\0') && (*buffer != ' ') && (*buffer != '\t')) {
            ++buffer;
        }
    }

    /*
     *  最後の引数の次にはヌルへのポインタを格納する
     */

    args[arg_index] = NULL;

    /*
     *  最後の引数をチェックして "&" ならば
     *
     *  "&" を引数から削る
     *  コマンドの状態を表す status に 1 を設定する
     *
     *  そうでなければ status に 0 を設定する
     */

    if(arg_index > 0 && strcmp(args[arg_index - 1], "&") == 0) {

        --arg_index;
        args[arg_index] = '\0';
        status = 1;

    } else {

        status = 0;

    }

    /*
     *  引数が何もなかった場合
     */

    if(arg_index == 0) {
        status = 3;
    }

    /*
     *  コマンドの状態を返す
     */

    return status;
}

/*----------------------------------------------------------------------------
 *
 *  関数名   : execute_command
 *
 *  作業内容 : 引数として与えられたコマンドを実行する
 *             コマンドの状態がフォアグラウンドならば、コマンドを
 *             実行している子プロセスの終了を待つ
 *             バックグラウンドならば子プロセスの終了を待たずに
 *             main 関数に返る（プロンプト表示に戻る）
 *
 *  引数     :
 *
 *  返り値   :
 *
 *  注意     :
 *
 *--------------------------------------------------------------------------*/

void execute_command(char *args[],    /* 引数の配列 */
                     int command_status)     /* コマンドの状態 */
{
    int pid;      /* プロセスＩＤ */
    int status;   /* 子プロセスの終了ステータス */   
  

       if(strcmp(args[0],"cd")==0){
        change_dir(args[1]);

      }  

     else if(strcmp(args[0],"pushd")==0){
        
       pushd_com();
      }
    

     else if(strcmp(args[0],"popd")==0){
        
       popd_com();
      }

     else if(strcmp(args[0],"prompt")==0){
        
       prompt=prompt_com(args[1]);
      }
     
      else if(strcmp(args[0],"alias")==0){
      
       alias_com(args[1] , args[2]);;
         
      } 

      else if(strcmp(args[0],"unalias")==0){
      
       unalias_com(args[1]);;
         
      } 

      else if(strcmp(args[0],"!!")==0){
      
       history_main(latest_com->prev->name);
         
      } 

      else if(args[0][0]=='!' && strcmp(args[0],"!!")!=0 && strcmp(args[0],"!")!=0){
      
       history_search2(args[0]);
       //printf("aaa\n");
         
      } 


    /*
     *  子プロセスを生成する
     *
     *  生成できたかを確認し、失敗ならばプログラムを終了する

     */
    else{ pid=fork();
     
     if(pid==-1){
           perror("fork");
     exit(0);
     }


    /******** Your Program ********/

    /*
     *  子プロセスの場合には引数として与えられたものを実行する
     *
     *  引数の配列は以下を仮定している
     *  ・第１引数には実行されるプログラムを示す文字列が格納されている
     *  ・引数の配列はヌルポインタで終了している
     */

if(pid==0){
      





     /* if(strcmp(args[0],"cd")==0){
        change_dir(args[1]);

      }*/
    
      if(strcmp(args[0],"test")==0){
        
       test_com();
      }

      /*if(strcmp(args[0],"pushd")==0){
        
       pushd_com();
      }*/

      else if(strcmp(args[0],"dirs")==0){
        
       dirs_com();
      }

      else if(strcmp(args[0],"history")==0){
        
       history_com();
      }

      else if(strcmp(args[0],"self_cat")==0){
        
       printf("   \n");
       self_cat(args[1]);
    
      }

   else {
   if(execv(args[0],&args[0])==-1){
      perror("execv");
            exit(1);
           }

    }


    






}
       
   



    /******** Your Program ********/

    /*
     *  コマンドの状態がバックグラウンドなら関数を抜ける
     */
   if(command_status==1){
    return ;

   }

    /******** Your Program ********/

    /*
     *  ここにくるのはコマンドの状態がフォアグラウンドの場合
     *
     *  親プロセスの場合に子プロセスの終了を待つ
     */
    if(pid>=1){
     wait(&status);
      //printf("\nchild process has finished\n\n");
     }




   }

    /******** Your Program ********/

    return;
}

void change_dir( char *input_name ){
char cu[256];

char *dir;
char *p;
char *ho;
int rc;
char *name;
int k;

if(input_name==NULL){

strcpy(cu,getenv("HOME"));


}

else{

if(*(input_name+0)=='/'){
 //printf("hoge2\n");
strcpy(cu,input_name);
 
}
 //rc=chdir(cu);
//printf("%s\n",cu);



if(*(input_name+0)!='/'){
for(k=0;input_name[k]!='\0';k++);
while(k>0){
input_name[k]=input_name[k-1];
k--;
}
input_name[0]='/';

 getcwd(cu,BUFLEN);

 /*if(cu==NULL){
 perror("getcwd");
 exit(EXIT_FAILURE);
 }*/


 strcat(cu,input_name);
}
}
//printf("%s\n",cu);

 if((chdir(cu))==-1){

 perror("chdir");
 return;
 }

getcwd(cu,BUFLEN);
printf("%s\n",cu);




}

void test_com(){
printf("test\n");
exit(EXIT_SUCCESS);
}

void dirs_com(void){
DIR_LIST *exp;

exp=root;
while(exp!=NULL){
printf("%s\n",exp->name);
exp=exp->next;
}
exit(EXIT_SUCCESS);

}


void pushd_com(void){
char cu2[256];

getcwd(cu2,BUFLEN);
root=new_item(cu2,root,NULL);
printf("pushd : %s\n",root->name);



}

DIR_LIST *new_item(char *name,  DIR_LIST *post_item, DIR_LIST *pre_item)
{
    DIR_LIST *new;

    new=malloc(sizeof(DIR_LIST));

    if (!new) {
        perror("new_item");
        exit(1);
    }

    if (name) {
        new->name = strdup(name);
        if (!new->name) {
            perror("new_item");
            exit(1);
        }
    }
   

    new->next=post_item;

    if(post_item!=NULL){
    post_item->prev=new;/*** [post_item の前の要素の指定（post_item が NULL でなければ）]***/
   }

     new->prev=pre_item;/*** [new の前の要素の指定] ***/
   
    if(pre_item!=NULL){/*** [pre_item の次の要素の指定（pre_item が NULL でなければ）] ***/
    pre_item->next=new;
 }

    return new;
}

DIR_LIST2 *new_item2(char *com_name,char *new_name , DIR_LIST2 *post_item, DIR_LIST2 *pre_item)
{
    DIR_LIST2 *new;

    new=malloc(sizeof(DIR_LIST2));

    if (!new) {
        perror("new_item");
        exit(1);
    }

    if (com_name) {
        new->com_name = strdup(com_name);
        if (!new->com_name) {
            perror("new_item");
            exit(1);
        }
    }

    if (new_name) {
        new->new_name = strdup(new_name);
        if (!new->new_name) {
            perror("new_item");
            exit(1);
        }
    }
   

    new->next=post_item;

    if(post_item!=NULL){
    post_item->prev=new;/*** [post_item の前の要素の指定（post_item が NULL でなければ）]***/
   }

     new->prev=pre_item;/*** [new の前の要素の指定] ***/
   
    if(pre_item!=NULL){/*** [pre_item の次の要素の指定（pre_item が NULL でなければ）] ***/
    pre_item->next=new;
 }

    return new;
}



void popd_com(void){
DIR_LIST *old;
char dir[256];

if(root==NULL){
return ;
}
old=root;

strcpy(dir,root->name);
change_dir(dir);
root=root->next;
free(old);


}

void history_inp( char *his ){
latest_com=new_item(his,NULL,latest_com);
if(latest_com->prev==NULL){
old_com=latest_com;
}

}

void history_com(void){
DIR_LIST *exp2;

exp2=old_com;
while(exp2!=NULL){
printf("%s",exp2->name);
exp2=exp2->next;
}
exit(EXIT_SUCCESS);

}

/*void history_do( void ){
char *com;

}*/

void clear_list(DIR_LIST *item){

	if(item==NULL){
		return ;
	}/*** [item が NULL なら戻る] ***/

	clear_list(item->next);/*** [clear_list の再帰呼出し] ***/

	free(item->name);/*** [ファイル名の領域を解放] ***/

	free(item); /*** [item の領域解放] ***/

}

void clear_list2(DIR_LIST2 *item){

	if(item==NULL){
		return ;
	}/*** [item が NULL なら戻る] ***/

	clear_list2(item->next);/*** [clear_list の再帰呼出し] ***/

	//free(item->com_name);/*** [ファイル名の領域を解放] ***/
	
      // free(item->new_name);

	free(item); /*** [item の領域解放] ***/

}


   

void history_main(char *command_buffer){
                char *args[MAXARGNUM];
                  int command_status;

history_inp(command_buffer); 

     /*
         *  入力されたバッファ内のコマンドを解析する
         *
         *  返り値はコマンドの状態
         */
  
         
        command_status = parse(command_buffer, args);

   

        execute_command(args, command_status);
    

    
}




void history_search2( char *st ){
char *comm;
DIR_LIST *search;
int i;
int x=0;
int y=0;

search=latest_com;


/*for(i=0;st[i+1]!='\0';i++){
comm[i]=st[i+1];
}
comm[i]='\0';*/
//printf("%s\n",st);


do{
x=0;
y=0;
search=search->prev;
//printf("%s\n",search->name);
while(st[x+1]!='\0'){
//printf("%c,%d\n",comm[x],x);
x++;
}
//printf("%d\n",x);

while(st[y+1]!='\0'){
if(st[y+1]==search->name[y]){
//printf("%c\n",search->name[y]);
y++;
}
else {
break ;
}

}
//printf("%d\n",y);

if(x==y){
history_main(search->name) ;
//printf("aaa\n");
break;
}

else if(search==old_com){
break ;
}
//printf("aa\n");
}while(search->prev!=NULL);



}

void alias_com(char *new_name , char *com_name){

latest_alias=new_item2(com_name,new_name,latest_alias,NULL);

printf("alias : command:%s  new name:%s\n",com_name,new_name);


}

char *alias_check(char *alias){

DIR_LIST2 *check;
check=latest_alias;
while(check!=NULL){
if(strcmp(check->new_name,alias)==0){
return check->com_name;
}
check=check->next;
}

return alias;
check=latest_alias;


}

void unalias_com(char *unalias){

DIR_LIST2 *check;
DIR_LIST2 *prev;
DIR_LIST2 *next;
check=latest_alias;
while(check!=NULL){
if(strcmp(check->new_name,unalias)==0){
//printf("aa\n");
break;
}
check=check->next;

}

if(check==latest_alias){
latest_alias=NULL;
}

prev=check->prev;
if(prev!=NULL){
prev->next=check->next;
}

next=check->next;
if(next!=NULL){
next->prev=check->prev;
}
free(check);





}

int wild_card_check(char *args[]){
int i=0;

while(args[i]!=NULL){
if(strcmp(args[i],"*")==0){

return(i);
}
i++;
}



return(-1);


}

void wild_card(char *args[], int n, int command_status){
  struct stat    filestat;
  struct dirent *directory;
  DIR           *dp;
   char args2[BUFLEN][MAXARGNUM];
   char *args3[MAXARGNUM];
  
  int m=n;
   int k,ll,i;
   int l=0;
   int p=0;
   int q=0; 
  while(args[l]!=NULL){
   strcpy(args2[l],args[l]);
    l++;
  }
 ll=l;

   dp = opendir(".");
  while((directory=readdir(dp))!=NULL){
    if(!strcmp(directory->d_name, ".") ||
       !strcmp(directory->d_name, ".."))
      continue;
    if(stat(directory->d_name,&filestat)==-1){
      perror("main");
      exit;

 }else{

if(l!=ll){
for(k=l-1;k>m;k--){
  strcpy(args2[k],args2[k-1]);
}
}  
   
  strcpy(args2[m],directory->d_name);
  l++;
  m++;  
}
}

  while(q<l-1){
   args3[p]=args2[p];
    p++;
    q++;
  }
q=0;
/*for(i=0;q<l-1;i++){
printf("%s\n",args3[i]);
q++;
}*/

execute_command(args3, command_status);



  closedir(dp);

  exit;
}


char *prompt_com(char *p){
if(p!=NULL){
return(p);
}

else{
 return("Command");
}

}






void self_cat(char *file_name){
FILE *fp;
char *co;

if((fp=fopen(file_name,"r"))==NULL){
fprintf(stderr,"オープン失敗\n",file_name);
exit;
}
while(fgets(co,BUFLEN,fp)!=NULL){
printf("%s",co);
}

fclose(fp);
exit(EXIT_SUCCESS);

}







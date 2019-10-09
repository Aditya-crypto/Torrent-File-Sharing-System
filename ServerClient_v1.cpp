#include<sys/types.h>
#include<sys/socket.h>
#include<bits/stdc++.h>
#include <netinet/in.h>  //for inet_addr()
#include <arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#include<cstdio>
#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#define BufferSize 50
#define PORT 5050
#define QUE 5
using namespace std;
struct chunk_info
{
     int chunk_no;
     char fname[50];
};
struct part_info
{   
    FILE *file_p;
    struct chunk_info chunkinfo;
    int portno;
};
int port;
void* recieve(void* arg)
{
     
     struct part_info* t1=(struct part_info*)malloc(sizeof(struct part_info));
     t1=(struct part_info*)arg;
     struct part_info t=*t1;
     cout<<"in recieve\n";
     int port=t.portno;
     int chunkno=t.chunkinfo.chunk_no;
     FILE* file_p_o=t.file_p;
     char filename[50];
     strcpy(filename,t.chunkinfo.fname);
    int sp=socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    memset(&addr,'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");
     int res =  connect(sp,(struct sockaddr*)&addr,sizeof(addr));
        if(res<0)
        {
          perror("errorrrrrrr");
          exit(0);
        }
        else
          cout<<"connected\n";
          struct chunk_info* Toserver=(struct chunk_info*)malloc(sizeof(struct chunk_info));
          Toserver->chunk_no=chunkno;
          strcpy(Toserver->fname,filename);
          cout<<"after connection "<<Toserver->chunk_no<<" "<<Toserver->fname<<"\n";
       if(send(sp,Toserver,sizeof(struct chunk_info),0)<0)
           perror("chunkinfo not send\n");
      // FILE *fp = fopen (filename,"wb");
          int offset=chunkno*BufferSize;
           fseek(file_p_o,offset,SEEK_SET);
          char Buffer[BufferSize] ; 
          // n=recv(sp, &file_size,sizeof(file_size),0);
          int n;
           n=recv(sp,Buffer,BufferSize,0);
                cout<<"hello" <<Buffer<<"\n";
                  fwrite(Buffer,sizeof(char),n,file_p_o);
	          memset(Buffer,'\0',BufferSize);
         cout<<"chunkno recieved: "<<chunkno<<"\n";
        fclose(file_p_o); 
        close(sp);
        pthread_exit(0);
}
void* client(void* arg)
{
    int num; 
    cin>>num;
    //struct part_info* chunk[4];
    struct part_info* chunk=(struct part_info*)malloc(sizeof(struct part_info)*4);
    pthread_t tids[4];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
     //struct part_info tr[4];
     int portlist[4]={1234,1235,1236,1237};
     FILE *fp = fopen ("hello.txt","wb");
          char Buffer[BufferSize]; 
          memset(Buffer,'a',BufferSize);
          long int file_size=200;
           int n=50;
          // n=recv(sp, &file_size,sizeof(file_size),0);
           while(file_size > 0)
           {      //cout<<"hello";
                  fwrite(Buffer,sizeof(char),n,fp);
	          memset(Buffer,'a',BufferSize);
                  file_size =file_size-n;
           } 
        fclose(fp);
          for(int i=0;i<4;i++)
          {  
             FILE *fp1 = fopen ("hello.txt","wb");
             chunk[i].file_p=fp1;
             chunk[i].portno=portlist[i];
             cout<<"in loop\n"; 
             chunk[i].chunkinfo.chunk_no=i;
             strcpy(chunk[i].chunkinfo.fname,"hello.txt");
             if(pthread_create(&tids[i],&attr,recieve,&chunk[i])<0)
                   perror("creation error");
          }
          for(int i=0;i<4;i++)
          {
             pthread_join(tids[i],NULL);
             cout<<"success thread "<<i<<"\n";
          }
    ////////////////////////
             /*chunk[0].portno=portlist[0];
             cout<<"not in loop\n"; 
             chunk[0].chunkinfo.chunk_no=0;
             strcpy(chunk[0].chunkinfo.fname,"hello.txt");
             if(pthread_create(&tids[0],&attr,recieve,&chunk[0])<0)
                    cout<<"fuck happened\n";
               pthread_join(tids[0],NULL);
            cout<<"success\n";
            
   /*       int n;
    FILE* fp=fopen("hello.txt","rb");
    fseek(fp,0L,SEEK_END);
    long int file_size=ftell(fp);
    rewind(fp);
    cout<<"client:"<<file_size<<"\n";
    if((n=send(sp,&file_size,sizeof(file_size),0))<0)
       perror("sending failed");
       else
       cout<<"sent successful"<<n<<"bytes\n";
       char buffer[BufferSize];
       while((n=fread(buffer,sizeof(char),BufferSize,fp))>0 && file_size>0)
           {    cout<<"heloooo\n";
                send(sp,buffer,n,0);
   	 	memset(buffer,'\0',BufferSize);
		file_size = file_size-n;
	   }
         cout<<"success"; 
    fclose(fp);
    close(sp);*/
}
///// server code
void* handle_client(void* arg)
{   
    int *t=(int*)arg;
    int connfd=*t;
    struct chunk_info* chunk=(struct chunk_info*)malloc(sizeof(struct chunk_info));
    if(recv(connfd,chunk,sizeof(struct chunk_info),0)<0)
            perror("receiving error\n");
          int n;
          FILE *fp = fopen(chunk->fname,"rb");
          int offset=(chunk->chunk_no)*BufferSize;
           fseek(fp,offset,SEEK_SET);
            char buffer[BufferSize];
            n=fread(buffer,sizeof(char),BufferSize,fp);
            cout<<"read n bytes"<<n;
            send(connfd,buffer,n,0);
   	    memset(buffer,'\0',BufferSize); 
   	    cout<<"sent chunk "<<chunk->chunk_no<<"\n";
	    fclose(fp);
         
         
         /* long int file_size;
           n=recv(connfd, &file_size,sizeof(file_size),0);
           if(n>0)
             cout<<"file_size recieved\n"<<"server:"<<file_size<<"\n";
           else
             perror("file_size recieved failed");
           while ( ( n=recv(connfd,Buffer,BufferSize,0) ) > 0  &&  file_size > 0)
           {      //cout<<"hello";
                  fwrite(Buffer,sizeof(char),n,fp);
	          memset( Buffer,'\0',BufferSize);
                  file_size =file_size-n;
           } 
        fclose(fp);*/
        //fflush(stdout); 
        pthread_exit(0);
    
}
void* server(void* arg)
{
     int *pt=(int*)arg;
     int port_s=*pt;
     pthread_t tid;
     pthread_attr_t attr;
     pthread_attr_init(&attr);
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    char sendBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    if(listen(listenfd, 10)<0)
    {
          perror("listening error\n");
    } 
    else
    cout<<"listening....\n";
    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
          if(connfd<0)
              perror("not accepted");
          if(pthread_create(&tid,&attr,handle_client,&connfd)<0)
             perror("creation error");
             pthread_join(tid,NULL);
        close(connfd);
       // fclose(fp);
        fflush(stdout); 
     }
     close(listenfd);
}
////// register user
void* creat_user(void* arg)
{       
     int sp=socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    memset(&addr,'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(PORT);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");
     int n;
    int res =  connect(sp,(struct sockaddr*)&addr,sizeof(addr));
        if(res<0)
        {
          perror("errorrrrrrr");
          exit(0);
        }
        else
          cout<<"connected\n";
        string username,passwd;
        cout<<"enter username\n";
        //getline(cin,username);
        cin>>username;
        cout<<"enter password\n";
         cin>>passwd;
        char user[100],pass[100];
         strcpy(user,username.c_str());
         strcpy(pass,passwd.c_str());
         if((n=send(sp,&user,sizeof(user),0))<0)
              perror("sending failed");
         if((n=send(sp,&pass,sizeof(pass),0))<0)
              perror("sending failed");
              char ack;
         recv(sp,&ack,sizeof(ack),0);
         if(ack=='#')
         cout<<"successfully registered\n";
         else
         cout<<"registration failed\n";
    close(sp);
    pthread_exit(0);
}
int main()
{
     pthread_t tid_c,tid_s;
     pthread_attr_t attr;
     pthread_attr_init(&attr);
     string what;
     cin>>port; 
    
     if(pthread_create(&tid_s,&attr,server,&port)<0)
           perror("creation error");
     if(pthread_create(&tid_c,&attr,client,NULL)<0)
           perror("creation error");
        pthread_join(tid_s,NULL);
        pthread_join(tid_c,NULL);
    /* cin>>what;
     if(what=="client")
     {
        if(pthread_create(&tid,&attr,client,NULL)<0)
             perror("creation error");
          pthread_join(tid,NULL);
         
     }
     else if(what=="client")
     {
         if(pthread_create(&tid,&attr,server,NULL)<0)
             perror("creation error");
          pthread_join(tid,NULL);
         
     }
     else if(what=="create_user")
     {
           if(pthread_create(&tid,&attr,creat_user,NULL)<0)
             perror("creation error");
             pthread_join(tid,NULL);
     }*/
     return 0;
}

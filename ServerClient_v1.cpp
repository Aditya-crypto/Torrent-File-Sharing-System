#include<sys/types.h>
#include<sys/socket.h>
#include<bits/stdc++.h>
#include <netinet/in.h>  //for inet_addr()
#include <arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#include<cstdio>
#include<stdio.h>
#define BufferSize 512*1024
#define PORT 5050
#define QUE 5
using namespace std;
int port;
void* client(void* arg)
{
      int sp=socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    memset(&addr,'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(PORT);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");

    int res =  connect(sp,(struct sockaddr*)&addr,sizeof(addr));
        if(res<0)
        {
          perror("errorrrrrrr");
          exit(0);
        }
        else
          cout<<"connected\n";
          int n;
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
    close(sp);
}
///// server code
void* handle_client(void* arg)
{   
    int *t=(int*)arg;
    int connfd=*t;
          int n;
          FILE *fp = fopen ("hello.txt","wb");
          char Buffer[BufferSize] ; 
          long int file_size;
           n=recv(connfd, &file_size,sizeof(file_size),0);
           if(n>0)
             cout<<"file_size recieved\n"<<"server:"<<file_size<<"\n";
           else
             perror("file_size recieved failed");
           while ( ( n=recv(connfd,Buffer,BufferSize,0) ) > 0  &&  file_size > 0)
           {    
                  fwrite(Buffer,sizeof(char),n,fp);
	          memset( Buffer,'\0',BufferSize);
                  file_size =file_size-n;
           } 
        fclose(fp);
        pthread_exit(0);
    
}
void* server(void* arg)
{
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
    serv_addr.sin_port = htons(PORT); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 
    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
          if(connfd<0)
              perror("not accepted");
          if(pthread_create(&tid,&attr,handle_client,&connfd)<0)
             perror("creation error");
          pthread_join(tid,NULL);
          close(connfd);
          fflush(stdout); 
     }
     close(listenfd);
}
int main()
{
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    string what;
    cin>>port;
    cin>>what;
    if(what=="client")
     {
        if(pthread_create(&tid,&attr,client,NULL)<0)
            perror("creation error");
          pthread_join(tid,NULL);
         
     }
     else
     {
         if(pthread_create(&tid,&attr,server,NULL)<0)
             perror("creation error");
         pthread_join(tid,NULL);
         
     }
    return 0;
}

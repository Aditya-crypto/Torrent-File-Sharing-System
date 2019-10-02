#include<sys/types.h>
#include<sys/socket.h>
#include<bits/stdc++.h>
#include <netinet/in.h>  //for inet_addr()
#include <arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<stdio.h>
#define PORT 5050
#define QUE 5
using namespace std;
void* handle_client(void* arg)
{   
    int *t=(int*)arg;
    int cl_fd=*t;
    char arr[10]="";
    recv(cl_fd,arr,sizeof(arr),0);
    cout<<arr;
    pthread_exit(0);
    //return NULL;
}

int main()
{
  pthread_t tid;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  int s_sp=socket(AF_INET,SOCK_STREAM,0);
   struct sockaddr_in s_address,c_address;
   s_address.sin_family=AF_INET;
   s_address.sin_port=htons(PORT);
   s_address.sin_addr.s_addr=inet_addr("127.0.0.1");
   int length=sizeof(sockaddr);
   if(bind(s_sp,(struct sockaddr*)&s_address,sizeof(s_address))<0)
   perror("binding error");
   listen(s_sp,QUE);
   
   while(1){
   int c_sp=accept(s_sp,(struct sockaddr*)&c_address,(socklen_t*)&length);
    if(c_sp<0)
    perror("not accepted");
    if(pthread_create(&tid,&attr,handle_client,&c_sp)<0)
       perror("creation error");
    pthread_join(tid,NULL);
    close(c_sp);
    fflush(stdout);
   }
    close(s_sp);
}

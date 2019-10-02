#include<sys/types.h>
#include<sys/socket.h>
#include<bits/stdc++.h>
#include <netinet/in.h>  //for inet_addr()
#include <arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#define QUE
using namespace std;
int port;
void client()
{
     int sp=socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    memset(&addr,'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");

    int res =  connect(sp,(struct sockaddr*)&addr,sizeof(addr));
        if(res<0){
        perror("errr");
        exit(0);}
    char arr[10]="hello";
    int n = send(sp,&arr,sizeof(arr),0);
    cout<<"Sent"<<n;
    //memset(&arr,'\0',10);
   // n = recv(sp,&arr,3,0);
   // cout<<"REcv"<<n;
   // cout<<arr;
    close(sp);
}
///// server code
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
void server()
{
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    int s_sp=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in s_address,c_address;
    s_address.sin_family=AF_INET;
    s_address.sin_port=htons(port);
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
int main()
{
    string what;
    cin>>port;
    cin>>what;
    if(what=="client")
    client();
    else
    server();
    return 0;
}

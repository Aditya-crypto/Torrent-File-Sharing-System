#include<sys/types.h>
#include<sys/socket.h>
#include<bits/stdc++.h>
#include <netinet/in.h>  //for inet_addr()
#include <arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#define PORT 5050
using namespace std;
int main()
{

    int sp=socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    memset(&addr,'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(PORT);
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
    return 0;
}

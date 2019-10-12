#include<sys/types.h>
#include<sys/socket.h>
#include<bits/stdc++.h>
#include <netinet/in.h>  //for inet_addr()
#include <arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#include<cstdio>
#include<stdio.h>
#include<map>
#include<vector>
#include<stdlib.h>

using namespace std;
map<string,string> mp; // map for user validation and new registrations
map<int,int> groupadmin;// map used for keeping admin info first: gid
map<int,vector<int> > groupusers;// map used for tracking all users in a group
map<int,vector<string> > lof;    //list of files in a group
map<string,vector<string> > fileinfo; //file information fn:size,loc,sha
map<string,vector<int> > fileusers;// list of users having that file
class RequestInfo
{
  public:
  int gid;
  int RequestPort;  
};
void* rgstr(void* arg)
{
          // pthread_detach(pthread_self());
    
           int *t=(int*)arg;
           int connfd=*t;
           int n;
           char user[100],pass[100];
           if(n=recv(connfd, &user,sizeof(user),0)<0)
                perror("username recieving failed\n");
           string username(user);
           if(n=recv(connfd, &pass,sizeof(pass),0)<0)
                perror("passwd recieving failed");
           string passwd(pass);
           mp[username]=passwd;
           char ack='#';
           send(connfd,&ack,sizeof(ack),0);
           cout<<"registered\n";
    pthread_exit(0);
}
void* verify(void* arg)
{
           int *t=(int*)arg;
           int connfd=*t;
           int n;
           char user[100],pass[100];
           if(n=recv(connfd, &user,sizeof(user),0)<0)
                perror("username recieving failed\n");
           string username(user);
           if(n=recv(connfd, &pass,sizeof(pass),0)<0)
                perror("passwd recieving failed");
           string passwd(pass);
           char ack;
           if(mp.find(username)!=mp.end())
            {      if(mp[username]==passwd)
                      {
                        ack='#';
                        send(connfd,&ack,sizeof(ack),0);
                        cout<<"verified\n";
                       }
             }
             else
             {  
                  ack='$';
                  send(connfd,&ack,sizeof(ack),0);
                  cout<<"verified\n";
             }
    pthread_exit(0);
}
void* groupCreation(void* arg)
{
           int *t=(int*)arg;
           int connfd=*t;
           int n,gid,adminport;
           if(n=recv(connfd, &gid,sizeof(int),0)<0)
                perror("gid recieving failed\n");
           if(n=recv(connfd, &adminport,sizeof(int),0)<0)
                perror("adminport recieving failed");
           char ack='#';
           
           groupadmin[gid]=adminport;
          // cout<<"adminport "<<groupadmin[gid]<<"\n";
           groupusers[gid].push_back(adminport);
           send(connfd,&ack,sizeof(ack),0);
           cout<<"group-created\n";
            fflush(stdout);
           pthread_exit(0);
}
void* SendToAdmin(void* arg)
{     
      //struct RequestInfo* info1=(struct RequestInfo*)malloc(sizeof(struct RequestInfo));
      RequestInfo* info1=new RequestInfo();
      //info1=(struct RequestInfo*)arg;
     info1=(RequestInfo*)arg;
      //struct RequestInfo info=*info1;
      int AdminGroup=info1->gid;
      int requestport=info1->RequestPort;
      //cout<<"RequestPort "<<info1->RequestPort<<" \n";
     int adminport=groupadmin[AdminGroup];
     //cout<<"adminport "<<adminport<<"\n";
    int sp=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    memset(&addr,'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(adminport);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");
     int n;
    int res =  connect(sp,(struct sockaddr*)&addr,sizeof(addr));
        if(res<0)
        {
          perror("errorrrrrrr");
          exit(0);
        }
        else
          cout<<"connected with admin\n";
          char ackr='J';
           while(1)
           {
            if(send(sp,&ackr,sizeof(ackr),0)<0)
            perror("ack not sent");
            else
            break;
           }
          if(send(sp,&requestport,sizeof(int),0)<0)
               perror("requestport sending failed\n");
         if(send(sp,&AdminGroup,sizeof(int),0)<0)
               perror("gid of admin sending failed\n");
          char ack;
          recv(sp,&ack,sizeof(ack),0);
         if(ack=='#')
         cout<<"request report to admin\n";
         else
         cout<<"not able to transfer request\n";
    close(sp);
    pthread_exit(0);
}
int main(int argc,char* argv[])
{
     int port=atoi(argv[1]);
     pthread_t tid_r;
     pthread_t tid_s;
     pthread_attr_t attr;
     pthread_attr_init(&attr);
     int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    //serv_addr.sin_addr.s_addr = htonl("127.0.0.1");
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(port); 

    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0)
        perror("binding error\n"); 

    if(listen(listenfd, 10)<0)
        perror("listening error"); 
    else
    cout<<"listening...\n";
   
     while(1){
     connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
      //struct RequestInfo* info=(struct RequestInfo*)malloc(sizeof(struct RequestInfo));
      RequestInfo info;//=new RequestInfo();
     char ack;
     recv(connfd,&ack,sizeof(ack),0);
     if(ack=='R')       //for registration
     {
        pthread_create(&tid_r,&attr,rgstr,&connfd);
        pthread_join(tid_r,NULL);
     }
     else if(ack=='V')     //to verify
     {
        pthread_create(&tid_r,&attr,verify,&connfd);
        pthread_join(tid_r,NULL);
     }
     else if(ack=='G')      //for group creation
     {
        pthread_create(&tid_r,&attr,groupCreation,&connfd);
        pthread_join(tid_r,NULL);
     }
     else if(ack=='J')        // join request from client
     {  
        //cout<<"in J\n";
        ack='#';
        int gid_i,requestport;
        recv(connfd,&gid_i,sizeof(int),0);
        recv(connfd,&requestport,sizeof(int),0);
        send(connfd,&ack,sizeof(ack),0);
        info.gid=gid_i;
        info.RequestPort=requestport;
        if(pthread_create(&tid_r,&attr,SendToAdmin,&info)<0)
           perror("sendToAdmin thread failed\n");
        pthread_join(tid_r,NULL);
     }
     else if(ack=='A')          //accept-request after work for the tracker
     {
        int gid,friendport;
        recv(connfd,&gid,sizeof(int),0);
        recv(connfd,&friendport,sizeof(int),0);
        //cout<<gid<<" "<<friendport;
        groupusers[gid].push_back(friendport);
        char ackr='#';
        if(send(connfd,&ackr,sizeof(ackr),0)<0)
            perror("ack not sent");
            fflush(stdout);
     } 
     else if(ack=='L')             // list groups
     {
              map<int,int>::iterator it ;
              int gid;
              int size=groupadmin.size();
              if(send(connfd,&size,sizeof(int),0)<0)
                      perror("size not sent");
             for(it =groupadmin.begin();it!=groupadmin.end();++it)
             {
                 gid=it->first;
                 if(send(connfd,&gid,sizeof(int),0)<0)
                      perror("gid not sent");
             }
     } 
     else if(ack=='U')          //accept-request after work for the tracker
     {
        int gid,friendport;
        recv(connfd,&gid,sizeof(int),0);
        recv(connfd,&friendport,sizeof(int),0);
        //cout<<gid<<" "<<friendport;
        //groupusers[gid].push_back(friendport);
        map<int,vector<int> >:: iterator it=groupusers.find(gid);
        if(it!=groupusers.end())
        {
             vector<int> v(it->second);
             vector<int>::iterator i;
             for(i=v.begin();i<v.end();i++)
             {
                cout<<*i<<" ";
             }
        }
        char ackr='#';
        if(send(connfd,&ackr,sizeof(ackr),0)<0)
            perror("ack not sent");
            fflush(stdout);
     }    
     else      
     cout<<"not coded\n";
     close(connfd);
     }
     close(listenfd);
}

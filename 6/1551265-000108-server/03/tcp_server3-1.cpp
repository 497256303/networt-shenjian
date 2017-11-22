#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
using namespace std;
#define QUEUE   20
#define BUFFER_SIZE 100
#define SEND_SIZE 10
#define TIMEOUT 1
int conn,num;
char buffer[BUFFER_SIZE],send_buf[SEND_SIZE];

void CbSigAlrm(int signo)
{
    send(conn, send_buf, sizeof(send_buf),MSG_DONTWAIT);
    cout<<"send=";
    int i;
    for(i=0;i<sizeof(send_buf);i++)
        cout<<send_buf[i];
    cout<<endl;
    alarm(TIMEOUT);
}

int main(int argc,char* argv[])
{
    ///����sockfd
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);
    int enable = 1;
    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	   perror("setsockopt(SO_REUSEADDR) failed");
    ///����sockaddr_in

    int port=atoi(argv[1]);
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(port);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(signal(SIGALRM,CbSigAlrm)==SIG_ERR)
    {
    perror("signal");
    return 0;
    }
    //�رձ�׼������л���ģʽ
    setbuf(stdout,NULL);

    int flags1 = fcntl(server_sockfd, F_GETFL, 0);                        //��ȡ�ļ���flags1ֵ��
    fcntl(server_sockfd, F_SETFL, flags1 | O_NONBLOCK);   //���óɷ�����ģʽ��

    ///bind���ɹ�����0����������-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }

    ///listen���ɹ�����0����������-1
    if(listen(server_sockfd,QUEUE) == -1)
    {
        perror("listen");
        exit(1);
    }

    ///�ͻ����׽���
    for(num=0;num<SEND_SIZE;num++)
        send_buf[num]=num+'0';
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    fd_set  fdR; 
    FD_ZERO(&fdR); 
    FD_SET(server_sockfd, &fdR); 
    ///�ɹ����طǸ������֣���������-1
    switch (select(server_sockfd + 1, &fdR, NULL,NULL, NULL)) //������connect
    {
        case -1:
            perror("select");
            break;/* ��˵��select�������� */
        case 0:
            sleep(1);
            printf("��ʱ\n");
            break; /* ˵�����趨��ʱ���ڣ�socket��״̬û�з����仯 */
        default:
            conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
    }
    cout<<"���ӳɹ�!"<<endl;
    
    int flags2 = fcntl(conn, F_GETFL, 0);                        //��ȡ�ļ���flags2ֵ��
    fcntl(conn, F_SETFL, flags2 | O_NONBLOCK);   //���óɷ�����ģʽ��
    
    alarm(TIMEOUT);

    while(1)
    {
        FD_ZERO(&fdR); 
        FD_SET(conn, &fdR); 
        switch (select(conn + 1, &fdR, NULL,NULL, NULL)) //������recv
        {
            case 0:
                break; /* ˵�����趨��ʱ���ڣ�socket��״̬û�з����仯 */
            default:
                memset(buffer,0,sizeof(buffer));
                int len=recv(conn, buffer, sizeof(buffer),MSG_DONTWAIT);
                if(len>0)
                {
                    cout<<"recv=";
                    for(num=0;num<len;num++)
                        cout<<buffer[num];
                    cout<<endl;
                }
        }
    }
    close(conn);
    close(server_sockfd);

    return 0;
}
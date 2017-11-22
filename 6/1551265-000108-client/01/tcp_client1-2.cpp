#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
using namespace std;
#define BUFFER_SIZE 1024


int main(int argc ,char* argv[])
{
    ///����sockfd
    int sock_cli = socket(AF_INET,SOCK_STREAM, 0);
    int port=atoi(argv[2]);
    ///����sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);  ///�������˿�
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);  ///������ip



    ///���ӷ��������ɹ�����0�����󷵻�-1
    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }
    else
	cout<<"���ӳɹ�!"<<endl;

    int flags1 = fcntl(sock_cli, F_GETFL, 0);                        //��ȡ�ļ���flags1ֵ��
    fcntl(sock_cli, F_SETFL, flags1 | O_NONBLOCK);   //���óɷ�����ģʽ��

    char recvbuf[BUFFER_SIZE];
    int flag;
    fd_set  fdR; 
        FD_ZERO(&fdR); 
        FD_SET(sock_cli, &fdR); 
        switch (select(sock_cli + 1, &fdR, NULL,NULL, NULL)) 
        {
            case -1:
                perror("select");
                break;/* ��˵��select�������� */
            case 0:
                sleep(1);
                printf("��ʱ\n");
                break; /* ˵�����趨��ʱ���ڣ�socket��״̬û�з����仯 */
            default:
            memset(recvbuf,0,sizeof(recvbuf));
            flag=recv(sock_cli, recvbuf, sizeof(recvbuf),MSG_DONTWAIT);
        }
    close(sock_cli);
    return 0;
}
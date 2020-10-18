#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>
#include<unistd.h>
#include<netdb.h>
struct node         //����ڵ�ṹ��
{
	int c;
	int a;
};
void* communication(void* arg);
int main()
{
	int sockfdto = socket(PF_INET,SOCK_STREAM,0);     //�����׽���
	assert(-1!=sockfdto);    //����ʧ�ܷ��ش���
	struct sockaddr_in serto,clito;     //�׽��ֵ�ַ
	memset(&serto,0,sizeof(serto));
	serto.sin_family=AF_INET;  //�����ַ��ΪIPv4
	serto.sin_port=htons(3000);    //���ö˿ں�
	struct hostent *host;	  //����host
	host = gethostbyname("192.168.0.105");     //���������IPv4��ַ
	serto.sin_addr=*((struct in_addr *)host -> h_addr);   ����IPv4��ַ 
	int resto=connect(sockfdto,(struct sockaddr*)&serto,sizeof(serto));    //��������
	assert(resto!=-1);   //����ʧ�ܷ��ش���
	int sockfd=socket(PF_INET,SOCK_STREAM,0);   //�����׽���
	assert(-1!=sockfd);    //����ʧ�ܷ��ش���
	struct sockaddr_in ser,cli;   //�׽��ֵ�ַ
	ser.sin_family=AF_INET;   //�����ַ��ΪIPv4
	ser.sin_port=htons(6000);   //���ö˿ں�
	ser.sin_addr.s_addr=htonl(INADDR_ANY);  //�����׽��ֵ�ַ 
	int res=bind(sockfd,(struct sockaddr*)&ser,sizeof(ser));  //���׽��������õĶ˿�
	assert(-1!=res);  //��ʧ�ܷ��ش���
	listen(sockfd,5);  //���ü����������������Ϊ5
	while(1)//��֤�������������Ӷ���ͻ���
	{
		int len=sizeof(cli);
		int c=accept(sockfd,(struct sockaddr*)&cli,&len);  //���ܿͻ��˵���������
        if(c<0)
        {
            printf("link error\n");
            continue;
        }
		pthread_t id;
		struct node *b;  //����ڵ�b
		b = (struct node *)malloc(sizeof(struct node));
		b->c = c;   //ָ��b->c��ֵ
		b->a = sockfdto;  //ָ��b->a��ֵ
		int n=pthread_create(&id,NULL,communication,(void*)b);//�����̣߳����ļ�������ǿתΪvoid*���˴�ֻ����ֵ���ݣ���ַ���ݵĻ������ܺ����̻߳�û�õ��õ�ַ��ֵ���ͱ����̸߳���
		assert(n==0);
	}
    close(sockfd);  //�ر�����
}
void* communication(void* arg)//�����߳������ͻ��˵Ľ���
{
	struct node *temp;  //����ڵ�temp
	temp = (struct node*)arg;
	while(1)//ʵ����һ���ͻ��˵Ķ�ν���
	{
	    char buff[1000]={0};
	    int c=temp->c;//���ļ�������ת��int��
	    int n=recv(c,buff,127,0);   //���տͻ��˵���Ϣ
	    if(n<=0)
	    {
			close(c);  //�ر�����
			printf("%d client over\n",c);
			break;
	    }
		send(temp->a, buff, strlen(buff), 0);   //������Ϣ��������
	    printf("%d:%s\n",c,buff);
	    char buffto[1000]={0};
	    int nto = recv(temp->a, buffto, 127, 0);   //���ܷ�������Ϣ
	    printf("%d:%s\n", temp->a, buffto);
		send(c, buffto, strlen(buffto), 0);  //������Ϣ���ͻ���
	}
}


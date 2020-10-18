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
#include<mysql/mysql.h>
void* communication(void* arg);
int main()
{
	int sockfd=socket(PF_INET,SOCK_STREAM,0);  //�����׽���
	assert(-1!=sockfd);   //����ʧ�ܷ��ش���
	struct sockaddr_in ser,cli;  //�׽��ֵ�ַ
	ser.sin_family=AF_INET;  //�����ַ��ΪIPv4
	ser.sin_port=htons(3000);   //���ö˿ں�
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
		int n=pthread_create(&id,NULL,communication,(void*)(long)c);//�����̣߳����ļ�������ǿתΪvoid*���˴�ֻ����ֵ���ݣ���ַ���ݵĻ������ܺ����̻߳�û�õ��õ�ַ��ֵ���ͱ����̸߳���
		assert(n==0);
	}
    close(sockfd);  //�ر�����
}
void* communication(void* arg)//�����߳������ͻ��˵Ľ���
{
	char sql[80]
  	int res;//ִ��sql����ķ��ر�־
  	MYSQL_RES *res_ptr;//ָ���ѯ�����ָ��
  	MYSQL_FIELD *field;//�ֶνṹָ��
  	MYSQL_ROW result_row;//���з��ز�ѯ��Ϣ
  	int row,column;//��ѯ���ص�����������
  	MYSQL *conn;//һ�����ݿ�����ָ��
  	conn = mysql_init(NULL);    //��ʼ�����Ӿ��
  	if(conn == NULL) 
	{ //�������NULL˵����ʼ��ʧ��
    	printf("mysql_init failed!\n");
    	return EXIT_FAILURE;
    }
  //����ʵ������
  //������conn���Ӿ����host��mysql���ڵ��������ַ,user�û���,passwd����,database_name���ݿ���,����Ķ���Ĭ��
  	conn = mysql_real_connect(conn,"192.168.0.105","root","123456","linuxtest",0,NULL,0);
  	if (conn) 
	{
    	printf("Connection success!\n");
  	} 
	else
	{
    	printf("Connection failed!\n");
  	}
  	mysql_query(conn,"set names gbk");//��ֹ���롣���ú����ݿ�ı���һ�¾Ͳ�������
	while(1)//ʵ����һ���ͻ��˵Ķ�ν���
	{
	    char buff[128]={0};
	    int c=(int)(long)arg;//���ļ�������ת��int��
	    int n=recv(c,buff,127,0);  //���տͻ��˵���Ϣ
	    if(n<=0)
	    {
			close(c);  //�ر�����
			printf("%d client over\n",c);
			break;
	    }
	    printf("%d:%s\n",c,buff);
//�ͻ��˷��͵���ϢΪһ�����ַ�����ͨ��ѭ������ͬ��Ϣ�ֿ����棬���������еķָ���"."ȥ��		
		int i;
		char temp[10][100];
		int j = 0;
		int k = 0;
		for(i = 0; i < strlen(buff); i++)
		{
			if(buff[i] != ' ')  //�������ո�
			{
				if(buff[i] != '.')  //������"."
				{
					temp[j][k] = buff[i];
					k = k + 1;
				}	
				else
				{
					continue;
				}
			}
			else
			{
				temp[j][k] = '\0';
				j = j + 1;
				k = 0;
			}	
		}
		if(j == 5)  //j=5�����ݿ�ִ�в������
		{	
			int temp1 = atoi(temp[1]);
			int temp2 = atoi(temp[2]);
			int temp3 = atoi(temp[3]);
			int temp4 = atoi(temp[4]);
			int temp5 = atoi(temp[5]);
			char sql[80];
			char str[80] = "insert into roomtable values('";
			strcat(str, temp[0]);  //��temp[0]���������ӵ�str��
			strcat(str, "', %d, %d, %d, %d, %d)");
			printf("%s", str);
			sprintf(sql, str, temp1, temp2, temp3, temp4, temp5);
			res = mysql_query(conn, sql);
			if(res == 0)  //����ɹ�
			{
				send(c,"insert", 6, 0);
			}
			else  //����ʧ��
			{
				send(c,"error", 5, 0);
			}
		}
		if(j == 0)  //j=0�����ݿ�ִ��ɾ������
		{
			char sql[80];
			char str[80] = "delete from roomtable where cust_name='";
			strcat(str, temp[0]);  //��temp[0]���������ӵ�str��
			strcat(str, "'");
			strcpy(sql, str);  //��str�е����ݸ��Ƶ�sql
			printf("%s", str);
			res = mysql_query(conn, sql);
			if(res == 0)  //ɾ���ɹ�
			{
				send(c,"delete", 6, 0);
			}
			else  //ɾ��ʧ��
			{
				send(c,"error", 5, 0);
			}
		}
		if(j == 1)  //j=1�����ݿ�ִ�в�ѯ����
		{
			char res_re[10000];
			int temp1 = atoi(temp[0]);
			int temp2 = atoi(temp[1]);
			memset(res_re, 0, sizeof(res_re));
			char sql[100];
			char str[100] = "select room_num from roomtable where in_time <= %d and out_time > %d and room_type = %d";
			int g, q;
			sprintf(sql, str, temp1, temp1, temp2);
			res = mysql_query(conn, sql);
			if(res)  //��ѯʧ��
			{
				perror("my_query");
				mysql_close(conn);
				exit(0);
			}
			else  //��ѯ�ɹ�
			{
				res_ptr = mysql_store_result(conn);
				if(res_ptr)
				{
					column = mysql_num_fields(res_ptr);
					row = mysql_num_rows(res_ptr);
					for(g = 1; g < row + 1; g++)  //�Է��ؽ��ÿһ�н��б���
					{
						result_row = mysql_fetch_row(res_ptr);
						for(q = 0; q < column; q++)	
						{
							strcat(res_re, result_row[q]);  //��result_row[q]����Ϣ���ӵ�res_re��
							strcat(res_re, " ");  //�Կո����
						}
					}
					send(c,res_re, sizeof(res_re), 0);  //��ͻ��˷��Ͳ�ѯ���
				}
			}	
		}
	}
}


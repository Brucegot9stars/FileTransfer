#include <iostream>
#include <sys/types/h>
#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <arpa/inet.h>


class Client
{

public:
	#define CLIENT_MAX_BUFFER_SIZE 1024
	#define CLIENT_MAX_FILENAME_SIZE 512
	
	
	Client(const char* ip_addr, int port);
	~Client(){}
	
public:
	int Init();
	int tranferData();
	const char * getFilename()
	{
		return file_name;	
	}
	
	
private:
	Client(){}
	void printProcess(int rate);
	
	
private:
	struct sockaddr_in  server_addr;
	struct sockaddr_in  client_addr;
	socklen_t server_addr_length;
	char buffer[CLIENT_MAX_BUFFER_SIZE];
	char file_name[CLIENT_MAX_FILENAME_SIZE + 1];
	int client_socket;
	int progress_rate;
	
};


/*********************************************************/
Client::Client(const char* ip_addr,int port)
{
	
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	if(port>=65535 || port<=0)
	{
		std::cout<<"Invalid server port!"<<std::endl;
		exit(1);
	}
	else if(NULL == ip_addr)
	{
		std::cout<<"Invalid server ip address!"	<<std::endl;
		exit(1);
	}
	
	server_addr.sin_port = htons(port);
	
	if( inet_pton(AF_INET, ip_addr, (void*)&server_addr.sin_addr) == 0)
	{
		std::cout<<"Invalid server address!"<<std::endl;
		exit(1);		
	}
	
	
	server_addr_length = sizeof(server_addr);
	memset(&buffer, 0, sizeof(buffer));
	memset(&file_name, 0, sizeof(file_name));
	progress_rate = 0;
}



Client::~Client()
{
	close(client_socket); 
}



int Client::Init()
{
	int ret = 0;
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;                // internetЭ����  
  client_addr.sin_addr.s_addr = htons(INADDR_ANY); // INADDR_ANY��ʾ�Զ���ȡ������ַ  
  client_addr.sin_port = htons(0);                 // auto allocated, ��ϵͳ�Զ�����һ�����ж˿� 
	
	do
	{
		
		client_socket = socket(AF_INET, SOCK_STREAM, 0);
		if(client_socket < 0)
		{  
  		std::cout<<"Create Socket Failed!"<<std::endl;  
  	  ret = -1;
  	  break;
  	}
  	
  	 // �ѿͻ��˵�socket�Ϳͻ��˵�socket��ַ�ṹ��   
  	if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))  
  	{  
			std::cout<<"Client Bind Port Failed!"<<std::endl;  
			ret = -1;
			break;
  	}   
  	
  	if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0)  
  	{  
  		std::cout<<"Can Not Connect To Server!"<<std::endl;
			ret = -1; 
			break;
  	} 
  
	}while(0);
	
	return ret;
	
}



int Client::tranferData()
{
	std::cout<<"Please Input File Name On Server."<<std::endl;  
  std::cin>>file_name;
	
	strncpy(buffer, file_name, strlen(file_name) > CLIENT_MAX_FILENAME_SIZE ? CLIENT_MAX_FILENAME_SIZE : strlen(file_name));  
  // �����������buffer�е����ݣ���ʱbuffer�д�ŵ��ǿͻ�����Ҫ���յ��ļ�������  
  send(client_socket, buffer, CLIENT_MAX_FILENAME_SIZE, 0);  
  
  
	FILE *fp = fopen(file_name, "w");  
	if (fp == NULL)  
	{  
		std::cout<<"File: "<<file_name<<" Can Not Open To Write!"<<std::endl;  
	  
	  exit(1);  
	}  
	
	// �ӷ������˽������ݵ�buffer��   
	memset(buffer, 0, CLIENT_MAX_BUFFER_SIZE);  
	int length = 0;  
	int write_length = 0;
	while(length = recv(client_socket, buffer, CLIENT_MAX_BUFFER_SIZE, 0))  
	{  
	  if (length < 0)  
	  {  
	  	  std::cout<<"Recieve Data From Server Failed!"<<std::endl;
	      break;  
	  }  
	
	  write_length = fwrite(buffer, sizeof(char), length, fp);  
	  if (write_length < length)  
	  {  
	  	  std::cout<<"Write File: "<<file_name<<" Failed!"<<std::endl;  
	      break;  
	  }  
	  memset(buffer, 0, CLIENT_MAX_BUFFER_SIZE);   
	}  
	
	std::cout<<"Recieve Data From Server Finshed!"<<std::endl;
	
	fclose(fp);  
	
 
}


void Client::printProcess(int rate)
{

	static char str[102] = { 0 };
	static const char* ptr = "|/-\\";

	if(rate != 100)
	{
		str[rate]='>';
		//- ����룬�ұ����ո�
		//%ms��mΪָ��������ֶεĿ�ȡ�������ݵ�λ��С��m������˲��Կո�������m����ʵ��λ�������
		//%-100s ��ʾ�������Ϊ100���ַ�������˶��䣬�������ÿո���䡣
		printf("[%-100s][%3d%%][%c]\r",str,rate,ptr[rate%4]);
		fflush(stdout);
		str[rate]='=';
	}
	else if(100 == rate)
	{
		str[rate] = '=';
		printf("[%-100s][%3d%%][ok]\n",str,rate);	
		memset(str,0,102);
	}

}
/*********************************************************/






int main(int argc,char* argv[])
{


		if (argc != 3 || argv[1] == NULL || argv[2] == NULL)  
		{  
				cout<<"Usage: "<<argv[0]<<" ServerIPaddress "<<"ServerPort"<<std::endl;
		    exit(1);  
		}  
		int port = atoi(argv[2]);
		
		
		Client fileRecvClient;
		fileRecvClient.Init(argv[1],port);
		fileRecvClient.tranferData();
		
    std::cout<<"Recieving File: "<<fileRecvClient.getFilename() <<" From Server Finished!\n"<<std::endl; 
  

		return 0;
}

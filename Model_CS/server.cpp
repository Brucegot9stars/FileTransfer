#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>

#define MAXBUFFERSIZE 1024
#define MAXFILENAMESIZE 512
#define DEFAULT_IP "192.168.100.2"
#define DEFAULT_PORT 6666
#define CCONFIG_FILE_ENV "OPENVPN_CLIENT_CONFIG_IFLE"


using namespace std;
//long long for off_t in 64bit system

string getfilepath(const char* env_var,const char* filename)
{
	const char* env = getenv(env_var);
	string ret;
	do
	{
		if(NULL == env_var || NULL == env)
		{
			
			std::cout<<"Environment variable does not exist!"<<std::endl;
			ret.clear();
			break;
		}
		else if(NULL == filename)
		{
			
			std::cout<<"You did't input a valid file name!"<<std::endl;
			ret.clear();
			break;
		}
		
		string _path(env);
		string _filepath=_path+'/'+filename;
		ret = _filepath;
		std::cout<<"get filepath: "<<ret<<std::endl;
		_path.clear();
		_filepath.clear();		
		
		
	}while(0);
	
	
	
	return ret;	
}
	
long long gitFileSize(const char* env_var, const char* tar_filename)
{
	long long ret = 0;
	do
	{/*
		const char* env = getenv(env_var);
		if(NULL == env_var || NULL == env)
		{
			
			std::cout<<"Environment variable does not exist!"<<std::endl;
			ret =  -1;
			break;
		}
		else if(NULL == tar_filename)
		{
			
			std::cout<<"You did't input a valid file name!"<<std::endl;
			ret = -1;
			break;
		}
		
		//judge the type of the file whether is a regular file
		string _filepath(env);
		string _filename=_filepath+'/'+tar_filename;
	*/
		
		struct stat fileifobuf;
		memset(&fileifobuf,0,sizeof(fileifobuf));

		int ret = stat(getfilepath(CCONFIG_FILE_ENV,tar_filename).c_str(), &fileifobuf);
		if(ret != 0)	
		{
			std::cout<<"You failed to get the file's information!"<<std::endl;
			ret = -1;
			break;
		}
		else if(S_ISREG(fileifobuf.st_mode))
		{
			//handle a regular file
			ret = fileifobuf.st_size;
			std::cout<<"filesize: "<<ret<<std::endl;
			break;
		}
		else
		{
			std::cout<<"We can not handle this kind of file!"<<std::endl;
			ret =  -1;
			break;
		}
		
	}while( 0 );
		
	
	return ret;
	
}

int main(int argc, char* argv[])
{
	string flag("yes");
	string server_port_str;
	int server_port = DEFAULT_PORT;

	if( argc<=1 )
	{
		std::cout<<"server [ip] port"<<std::endl;
		return -1;
	}
	else if(argv[2] != NULL)
	{
		server_port_str.assign(argv[2]);
	}
	else
	{
		return -1;
	}

	while(1)
	{
		server_port = atoi(server_port_str.c_str());
		if(server_port > 65535 || server_port <= 0 )
		{
			std::cout<<"Invalid port: "<< server_port_str <<std::endl;
			std::cout<<"Would you like to use the default port:6666 or change to another?[default:yes]/[no]"<<std::endl;
			std::cin>>flag;
			if(flag.compare("no") == 0)
			{
				flag.assign("yes");
				std::cin>>server_port_str;
				if(server_port_str.empty())
				{
					server_port_str.assign("-1");
					continue;
				}
				continue;		
			
			}
			else
			{
				server_port = DEFAULT_PORT;
				std::cout<<"Using default port!"<<std::endl;
				break;
			}
		}
		else
		{
			break;
		}
	}

	struct sockaddr_in server_addr;
	memset(&server_addr,0,sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	inet_pton(AF_INET,argv[1],(void*)&server_addr.sin_addr.s_addr);


	int server_socket = socket(AF_INET, SOCK_STREAM, 0); 
	if( server_socket < 0 )
	{
		std::cout<<"Create Socket Failed!"<<std::endl;
		exit(1);
	}

	if(bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)))
	{
		std::cout<<"server bind port: "<<server_port<<" failed!"<<std::endl;
		exit(1);
	}

	if(listen(server_socket,20))
	{	
		std::cout<<"Server listen failed!"<<std::endl;
		exit(1);
	}
	
	struct sockaddr_in client_addr;
	socklen_t addr_length = sizeof(client_addr);
	int data_length = 0;
	int conn_socket = -1;
	char buffer[MAXBUFFERSIZE] = { 0 };
	char filename[MAXFILENAMESIZE+1] = { 0 };
	long long filesize = 0;

	while(1)
	{
		conn_socket = accept(server_socket,(struct sockaddr*)&client_addr, &addr_length);
		if(conn_socket <0 )
		{
			std::cout<<"Connection failed!"<<std::endl;
			break;
		}

		data_length = recv(conn_socket, buffer, MAXBUFFERSIZE, 0);
		if(data_length < 0)
		{	
			break;
		}
		
		//获取客户端发来的文件名
		strncpy(filename,buffer,strlen(buffer)>MAXFILENAMESIZE?MAXFILENAMESIZE:strlen(buffer));
			
		FILE* fp = fopen(getfilepath(CCONFIG_FILE_ENV,filename).c_str(), "r");
		if(fp == NULL)
		{
			std::cout<<"File: "<<filename<<" does not exit!"<<std::endl;
			std::cout<<getfilepath(CCONFIG_FILE_ENV,filename)<<std::endl;
			//memset(filename,0,MAXFILENAMESIZE+1);
		}
		else
		{
			//获取文件的大小,考虑文件大小不合格？
			memset(buffer, 0, MAXBUFFERSIZE);
			filesize = gitFileSize(CCONFIG_FILE_ENV, filename);
			sprintf(buffer,"%lld",filesize);
			send(conn_socket, buffer, sizeof(buffer),0);
		
			
			memset(buffer, 0, MAXBUFFERSIZE);
			int file_block_length = 0;
			while((file_block_length = fread(buffer, sizeof(char),MAXBUFFERSIZE,fp)) > 0)
			{
				if(send(conn_socket, buffer, file_block_length,0)<0)
				{
					std::cout<<"Failed to send file: "<<filename<<std::endl;
					break;
				}
				memset(buffer, 0, MAXBUFFERSIZE);
			}
			fclose(fp);
			std::cout<<"Transfer File completed!"<<std::endl;
	
		}
		memset(filename,0,MAXFILENAMESIZE+1);
		close(conn_socket);

	}
	close(server_socket);
	return 0;

}

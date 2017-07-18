#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std ;



/* int readFile1(string env_var,string _filename)
{
	string _filepath = getenv(env_var.c_str());
	if(_filepath.empty())
	{
		std::cout<<"Environment variable does not exist!"<<std::endl;
		return -1;
	}
	else if(_filename.empty())
	{
		std::cout<<"You did't input a valid file name!"<<std::endl;
		return -2;
	}
	
	//judge the type of the file whether is a regular file
	
	_filename=_filepath+'/'+_filename;
	
	struct stat fileifobuf;
	memset(&fileifobuf,0,sizeof(fileifobuf));

	int ret = stat(_filename.c_str(), &fileifobuf);
	if(ret != 0)	
	{
		std::cout<<"You failed to get the file's information!"<<std::endl;
		return -3;
	}
	else if(S_ISREG(fileifobuf.st_mode)
	{
		//handle a regular file
	}
	else
	{
		std::cout<<"We can not handle this kind of file!"<<std::endl;
		return -4;
	}
	return 0;
} */


int readFile2(const char* env_var,const char* tar_filename)
{
	int ret = 0;
	do
	{
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
			ret =  -2;
			break;
		}
		
		//judge the type of the file whether is a regular file
		string _filepath(env);
		string _filename=_filepath+'/'+tar_filename;
	
		struct stat fileifobuf;
		memset(&fileifobuf,0,sizeof(fileifobuf));

		int ret = stat(_filename.c_str(), &fileifobuf);
		if(ret != 0)	
		{
			std::cout<<"You failed to get the file's information!"<<std::endl;
			ret = -3;
			break;
		}
		else if(S_ISREG(fileifobuf.st_mode))
		{
			//handle a regular file
			std::cout<<tar_filename<<"'s size: "<<fileifobuf.st_size<<std::endl;
			break;
		}
		else
		{
			std::cout<<"We can not handle this kind of file!"<<std::endl;
			ret =  -4;
			break;
		}
		
	}while( 0 );
		
	
	return ret;
}
int main(int argc,char* argv[])
{

	int ret = readFile2("OPENVPN_CLIENT_CONFIG_IFLE","cfgfiles.7z");
	return ret;

}

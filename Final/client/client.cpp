#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <cstring>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>


#define BUFFER_SIZE 2048

using namespace std;

int main(int argc, char* argv[])
{
	int fd,portNo;
	struct sockaddr_in server_address;
	
	// Check command line args
	if( argc < 3)
	{
		cout<<"Syntax => client <hostname> <port>"<<endl;
		return 0;
	}

	portNo = atoi(argv[2]);

	// Create socket
	fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( fd < 0 )
	{
		cout<<"Could not create socket!"<<endl;
		return 0;
	}
	
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons( portNo );

	// Convert hostname
	if( inet_pton(AF_INET, argv[1] , &server_address.sin_addr) < 0)
	{
		cout<<"Invalid hostname"<<endl;
		return 0;
	}

	// Connect
	if( connect(fd, (struct sockaddr *) &server_address, sizeof(server_address)) )
	{
		cout<<"Could not connect!"<<endl;
		return 0;
	}

	char buffer[BUFFER_SIZE];
	
	do
	{
		bzero(buffer,BUFFER_SIZE);
		read(fd,buffer,BUFFER_SIZE);

		string recieved = string(buffer);

		cin.getline(buffer,BUFFER_SIZE);

		write(fd,buffer,strlen(buffer));

		if( string(buffer).find("filedown") == 0 )
		{
			string filename = string(buffer).substr(string(buffer).find("filedown ")+1);
			FILE *file = fopen(filename.c_str(),"w");
			if(!file)
			{
				while( recv(fd,buffer,BUFFER_SIZE,0) )
				{
					fwrite(buffer,strlen(buffer),1,file);
				}
				fclose(file);
			}
			else 
			{
				cout<<"Could not create file";
			}
		}
		else if( string(buffer).find("fileup") == 0 )
		{
			string filename = string(buffer).substr(string(buffer).find("fileup ")+1);
			FILE *file = fopen(filename.c_str(),"r");
			if(!file)
			{
				while( fread(buffer,BUFFER_SIZE,1,file) )
				{
					write(fd,buffer,strlen(buffer));
				}
				fclose(file);
			}
			else 
			{
				cout<<"File not found!";
			}
		}

	} while( string(buffer) != "exit");

	close(fd); 

}
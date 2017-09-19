#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <cstring>
#include <arpa/inet.h>

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
		cout<<argv[1]<<": ";
		
		bzero(buffer,BUFFER_SIZE);
		cin.getline(buffer,BUFFER_SIZE);

		write(fd,buffer,BUFFER_SIZE);

		bzero(buffer,BUFFER_SIZE);
		read(fd,buffer,BUFFER_SIZE);

		cout<<string(buffer)<<endl;

	} while( string(buffer) != "exit");

	close(fd); 

}
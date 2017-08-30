#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 8080

using namespace std;

int main()
{
	struct sockaddr_in server_address;
	int addrlen = sizeof(server_address);
	char buffer[1024] = {0};
	char msg[1024] = {0};

	// Create Socket
	int fd = socket(AF_INET,SOCK_STREAM,0);
	( fd < 0 ) ? cout<<"Error. Could not create socket.\n" : cout<<"Socket created successfully.\n";

	memset(&server_address, '0', sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons( PORT );
	
	// Conversion
	int addr = inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);
	(addr < 0 ) ? cout<<"Invalid address. Address not supported \n" : cout<<"Conversion successful\n";
	
	// Connect Socket
	int c = connect(fd, (struct sockaddr *) &server_address, sizeof(server_address));
	( c < 0 ) ? cout<<"Error. Could not connect to socket.\n"<<strerror(errno)<<endl : cout<<"Socket connected.\n";

	// Read
	int valread = read( fd , buffer, 1024);
    cout<<"Recieved message => "<<buffer<<endl;

    // Send
    while(strcmp(msg,"close")!=0){
    	cout<<"Enter message to send => ";
		cin>>msg;
   		send(fd,msg,strlen(msg),0);
    }
    close(fd);
   	

}
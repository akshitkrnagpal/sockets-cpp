#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#define PORT 8080

using namespace std;

int main()
{
	struct sockaddr_in server_address;
	int addrlen = sizeof(server_address);
	int opt = 1;
	char buffer[1024] = {0};
	char connected_msg[] = "Connected to server. Please send something.\n";

	// Create Socket
	int fd = socket(AF_INET,SOCK_STREAM,0);
	( fd < 0 ) ? cout<<"Error. Could not create socket.\n" : cout<<"Socket created successfully.\n";

	int sso = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	( sso < 0 ) ? cout<<"Error. Could not SSO.\n" : cout<<"SSO successful.\n";
	
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons( PORT );

	// Bind Socket
	int b = bind(fd, (struct sockaddr *) &server_address, sizeof(server_address));
	( b < 0 ) ? cout<<"Error. Could not bind to socket.\n"<<strerror(errno)<<endl : cout<<"Socket binded.\n";

	// Listen
	int l = listen(fd,5);
	( l < 0 ) ? cout<<"Error. Could not listen.\n" : cout<<"Waiting for connection.\n";

	while(true) {

		// Accept
		int new_socket = accept(fd, (struct sockaddr *) &server_address, (socklen_t*) &addrlen);
		( new_socket < 0 ) ? cout<<"Error. Could not accept.\n" : cout<<"Client connected.\n";

		// Send
		send(new_socket,connected_msg,strlen(connected_msg),0);

		int valread;
		bool readBuffer = true;
		while( readBuffer ) {
			cout<<readBuffer;
			int valread = read(new_socket,buffer,1024);
			cout<<"Recieved message => "<<buffer<<(strcmp(buffer,"close")==0)<<endl;
			if (strcmp(buffer,"close")==0) { readBuffer=false; }
			cout<<readBuffer;
		}
	
		cout<<"Client disconnected";

	}
	

	

}
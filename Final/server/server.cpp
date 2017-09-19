#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <cstring>
#include <sstream>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048

using namespace std;

static int new_socket;

void * Menu(void * tid);

int main(int argc, char* argv[])
{
	int portNo,fd,opt=1;
	struct sockaddr_in server_address, client_address;
	
	pthread_t threadArray[MAX_CLIENTS];

	// Check command line args
	if( argc < 2 )
	{
		cout<<"Syntax => server <port>"<<endl;
		return 0;
	}
	portNo = atoi(argv[1]);

	// Create socket
	fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( fd < 0 )
	{
		cout<<"Could not create socket!"<<endl;
		return 0;
	}
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons( portNo );

	// Bind socket
	if ( bind(fd, (struct sockaddr *) &server_address, sizeof(server_address)) )
	{
		cout<<"Could not bind!"<<endl;
		return 0;
	}

	// Listen
	listen(fd,5);

	cout<<"Server running at port "<<portNo<<endl;

	for(int i=0; i < MAX_CLIENTS; i++)
	{
		socklen_t len = sizeof(client_address);

		// Accept
		new_socket = accept(fd, (struct sockaddr *) &client_address, (socklen_t*) &len);
		if( new_socket < 0)
		{
			cout<<"Could not accept connection!"<<endl;
			return 0;
		}
		
		// Create thread
		pthread_create(&threadArray[i],NULL,Menu,NULL);
	}

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		pthread_join(threadArray[i],NULL);
	}


}

void * Menu(void * tid)
{
	int clientId = pthread_self();
	cout<< "Client Connected (with id = "<< clientId <<" )"<<endl;
	char buffer[BUFFER_SIZE];
	string command(buffer);
	
	do
	{
		bzero(buffer,BUFFER_SIZE);
		read(new_socket,buffer,BUFFER_SIZE);

		command = string(buffer);
		if(command.length() > 0)
		{
			if(command.find(" "))
			{
				string operation = command.substr(0,command.find(' '));
				stringstream operands(command.substr(command.find(' ')+1));
				
				int result,n;
				size_t pos = 0;

				if(operation == "neg")
				{
					operands >> n;
					result = -n;
				}
				else if (operation == "add")
				{
					result = 0;
					while( operands >> n )
					{
						result+=n;
					}
				}
				else if (operation == "mul")
				{
					result = 1;
					while( operands >> n )
					{
						result*=n;
					}
				}
				
				string response;
				if( result )
				{
					stringstream response_stream;
					response_stream << result;
					
					response = response_stream.str();
				}
				else
				{
					response = "No command \""+operation+"\" found.";
				}
				
				strcpy(buffer,response.c_str());
				send(new_socket,buffer,strlen(buffer),0);
			}

		}


	} while(command != "exit");
	
	cout<< "Client disconnected (with id = "<< clientId <<" )"<<endl;
}

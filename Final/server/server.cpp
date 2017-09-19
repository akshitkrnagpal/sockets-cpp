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

#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048

using namespace std;

static int new_socket;

void * Menu(void * tid);

int file_select(const struct dirent *entry)
{
	if ((entry->d_name == ".") ||(entry->d_name == ".."))
		return 0;
	else
		return 1;
}

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
		getcwd(buffer,BUFFER_SIZE);
		string current_directory(buffer);
		write(new_socket,buffer,strlen(buffer));
		write(new_socket,": ",2);
		
		bzero(buffer,BUFFER_SIZE);
		read(new_socket,buffer,BUFFER_SIZE);

		command = string(buffer);
		if(command.length() > 0)
		{
			if(command.find(" "))
			{
				string operation = command.substr(0,command.find(' '));
				stringstream params(command.substr(command.find(' ')+1));
				string temp;
				
				int result,n;
				size_t pos = 0;
				bool flag = false;

				if(operation == "neg")
				{
					params >> n;
					result = -n;
					flag=true;
				}
				else if (operation == "add")
				{
					result = 0;
					while( params >> n )
					{
						result+=n;
					}
					flag=true;
				}
				else if (operation == "mul")
				{
					result = 1;
					while( params >> n )
					{
						result*=n;
					}
					flag=true;
				}

				string response;
				if( flag )
				{
					stringstream response_stream;
					response_stream << result;
					
					response = response_stream.str();
				}
				else
				{
					if(operation == "pwd")
					{
						getcwd(buffer,BUFFER_SIZE);
						response = string(buffer);
					}
					else if(operation == "cd")
					{
						params >> temp;
						chdir(temp.c_str());
					}
					/*else if( operation == "ls")
					{
						//cout<<"asdnasd";
						struct dirent **files;
						int file_select(const struct dirent*);
						char pathname[100];

						int count = scandir(pathname, &files, file_select, alphasort);

						response="";
						for (int i=0;i<count;i++)
						{
							response+=string(files[i]->d_name)+"\n";
						}
					}*/
					else if( operation == "filedown" )
					{
						char filename[50];
						params >> filename;

						FILE *file = fopen(filename,"r");
						if(!file)
						{
							while( fread(buffer,BUFFER_SIZE,1,file) )
							{
								write(new_socket,buffer,strlen(buffer));
							}
							fclose(file);
						}
						else 
						{
							response = "File not found!";
						}
					}
					else if( operation == "fileup" )
					{
						char filename[50];
						params >> filename;

						FILE *file = fopen(filename,"w");
						if(!file)
						{
							while( recv(new_socket,buffer,BUFFER_SIZE,0) )
							{
								fwrite(buffer,strlen(buffer),1,file);
							}
							fclose(file);
						}
						else 
						{
							response = "Could not create file!";
						}
					}
				}
				
				strcpy(buffer,response.c_str());
				write(new_socket,buffer,strlen(buffer));
			}

		}


	} while(command != "exit");
	
	cout<< "Client disconnected (with id = "<< clientId <<" )"<<endl;
}

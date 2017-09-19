# Sockets in CPP

Implementation of Sockets in c++

## Task

To create programs of server and client using sockets that can do the following.

* Basic Client-Server
* Client-Server message send/recieve
* Client-Server message send/recieve on different machines
* Multiple client connections
* File upload/download
* Any 5 commands specified on client to execute on server

## How to start

#### Server
`./server <port>`

#### Client
`./client <hostname> <port>`
  
## Compiling

#### Server
`g++ -pthread server.cpp -o server`
 
#### Client
`g++ client.cpp -o client`

> Server uses **pthread** to allow multiple client connections, hence `-pthread` is required to compile it.

## Commands

Commands available once the connection is established.

### Filesystem

#### Get current directory
`pwd`

#### Change current directory
`cd <directory>`

#### List all file of directory
`ls`

### Upload/Download

#### Upload
`fileup <path/filename>`
The file from client machine is saved on server machine where server executable is placed.

#### Download
`filedown <path/filename>`
The file from server machine is saved on client machine where client executable is placed.

### Arithmatic
(Calculation done on server)

#### Negation
`neg <integer>`
Returns negative of integer provided.

#### Addition
`add <int> <int> <int> ....`
Returns sum of all the integers provided.

#### Multiplication
`mul <int> <int> <int> ....`
Return product of all the integers provided.

> More to be added soon







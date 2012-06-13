//============================================================================
// Name        : ChatterBeef.cpp
// Author      : bearpaw7
// Version     :
// Copyright   : MIT License
// Description : ChatterBeef Server
//============================================================================

#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <pthread.h>
#include <sys/socket.h>				// socket()
#include <netinet/in.h>				// sockaddr_in type

//#include <sys/types.h>
//#include <arpa/inet.h>
#include <netdb.h>

/*************************************/
int SERVERPORT = 5000;			// Port number to listen on
int LISTENQUEUE = 10;			// Number of incoming connections to queue
int MESSAGEBUFFER = 1024;		// Maximum data received per message
/*************************************/

using namespace std;

void* handleNewConnection(void* voidConnectFd){
	pthread_detach(pthread_self());

	int connectFd = *((int*) voidConnectFd);
	char chBuffer[MESSAGEBUFFER];
	bzero(&chBuffer, MESSAGEBUFFER);
	int chReceived = read(connectFd, &chBuffer, MESSAGEBUFFER);
	if(chReceived < 0){
		perror("Transmission error from client");
	}else if(chReceived == 0){
		cout << "Client closed connection" << endl;
	}else{
		cout << "Received " << chReceived << " characters: " << chBuffer << endl;

		string reply = "Received ";
		stringstream ss;
		ss << chReceived;
		reply += ss.str() + " characters\n";
		int chSent = write(connectFd, reply.c_str(), strlen(reply.c_str()));
		cout << "Replied with " << chSent << " characters" << endl;
	}

	shutdown(connectFd, 2);
	pthread_exit(NULL);
}

void listenLoop(int _listenSocket){
	if( _listenSocket <= 2){
		perror("Bad socket to receive clients");
		return;
	}

	while(true){
		sockaddr_in clientSocket;
		int len = sizeof(clientSocket);
		int connectFd = accept(_listenSocket, (struct sockaddr*) &clientSocket,
				(socklen_t *) &len);
		if(connectFd < 0){
			perror("Failed to accept incoming connection");
			continue;
		}
		cout << "New client connected" << endl;

		pthread_t connectThread;
		if(pthread_create(&connectThread, NULL, handleNewConnection, (void*) &connectFd ) != 0){
			perror("Failed to create new thread for incoming request");
			close(connectFd);
			continue;
		}
	}

}

int createListenSocket(){
	int listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenFd < 0){
		perror("Bad socket");
		return listenFd;
	}

	sockaddr_in serverAddress;
	bzero((char*)&serverAddress, sizeof(sockaddr_in));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVERPORT);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	int bindTest = bind(listenFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if(bindTest < 0){
		perror("Bind to port failed");
		return bindTest;
	}

	int listenTest = listen(listenFd, LISTENQUEUE);
	if(listenTest <0){
		perror("Listen for incoming connections failed");
		return listenTest;
	}

	char chHostname[100];
	if(gethostname(chHostname, 100) < 0){
		perror("Failed to get hostname");
	}else{
		cout << "Listening on " << chHostname << ", " <<
				(gethostbyname(chHostname))->h_name << endl;
	}

	return listenFd;
}


int main() {
	cout << "hello world" << endl; // prints hello world

	listenLoop( createListenSocket() );

	return 0;
}


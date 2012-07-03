//============================================================================
// Name        : ChatterBeef.cpp
// Author      : bearpaw7
// Version     :
// Copyright   : MIT License
// Description : ChatterBeef Server
//============================================================================

#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <pthread.h>
#include <sys/socket.h>				// socket()
#include <netinet/in.h>				// sockaddr_in type
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "ChatBox.h"
#include <vector>

/*************************************/
enum{
	SERVERPORT = 5004,			// Port number to listen on
	LISTENQUEUE = 10,			// Number of incoming connections to queue
	MESSAGEBUFFER = 1024,		// Maximum data received per message
	MAXROOMS = 10,				// Max number of chatrooms to create
	MAXCLIENTS = 10,			// Max number of clients per room
};
/*************************************/

//using namespace std;  // It has been suggested to me that this line is bad.
using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;

vector<ChatBox*> chatList(MAXROOMS, NULL);

void* handleNewConnection(void* voidConnectFd){
	pthread_detach(pthread_self());
	int connectFd = *((int*) voidConnectFd);

	string hello = "ChatterBeef Server\nMenu:\nCreate <name>\nReview\nDestroy <name>\n";
	int bytesSent = send(connectFd, hello.c_str(), strlen(hello.c_str()), 0);
	if(bytesSent <= 0){
		cout << "New connection closed after welcome message";
		return NULL;
	}

	char chBuffer[MESSAGEBUFFER];
	bzero(&chBuffer, MESSAGEBUFFER);
	int bytesReceived = recv(connectFd, &chBuffer, MESSAGEBUFFER, 0);
	if(bytesReceived < 0){
		perror("Transmission error from client");
	}else if(bytesReceived == 0){
		cout << "Client closed connection" << endl;
	}else{
		cout << "Received " << bytesReceived << " bytes: " << chBuffer << endl;

		string reply = "Received ";
		stringstream ss;
		ss << bytesReceived;
		reply += ss.str() + " characters\n";
		int bytesSent = send(connectFd, reply.c_str(), strlen(reply.c_str()), 0);
		cout << "Replied with " << bytesSent << " bytes" << endl;
	}
	close(connectFd);
	pthread_exit(NULL);
	return NULL;
}

void listenLoop(int _listenSocket){
	if( _listenSocket <= 2){
		perror("Bad socket to receive clients");
		return;
	}
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	fd_set masterfds;
	fd_set tempfds;
//	FD_ZERO(&masterfds);					// Compiler error : '__builtin_bzero' could not be resolved
	bzero(&masterfds, sizeof(masterfds));	// work-around
	FD_SET(_listenSocket, &masterfds);
	int maxfd = _listenSocket;

	while(true){
		cout << "loop\n";
		tempfds = masterfds;
		int selectInt = select(maxfd+1, &tempfds, NULL, NULL, &timeout);
//		if(select(maxfd, &tempfds, NULL, NULL, &timeout) == -1){
		if(selectInt < 0){
			perror("Error in main loop select()");
			exit(1);
		}
		cout << "select int = " << selectInt << endl;
//		for(int i=0; i<maxfd; ++i){
			if(FD_ISSET(_listenSocket, &tempfds)){
				sockaddr_in clientSocket;
				int len = sizeof(clientSocket);
				int connectFd = accept(_listenSocket, (struct sockaddr*) &clientSocket, (socklen_t *) &len);
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
//		}
	}
}

int main() {
	cout << "hello world" << endl; // prints hello world

	listenLoop( ChatBox::createListenSocket( SERVERPORT, LISTENQUEUE) );

	return 0;
}


/*
 * ChatBox.cpp
 *
 *  Created on: Jul 1, 2012
 *      Author: met898
 */

#include "ChatBox.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <netdb.h>


ChatBox::ChatBox(int _port, int _listenQueue, std::string _name, int _maxClients) {
	int listenSocket = createListenSocket(_port, _listenQueue);
	if(listenSocket < 0){
		return;
	}
	this->port = _port;
	this->name = _name;
	this->maxClients = _maxClients;
}

ChatBox::~ChatBox() {
	// TODO Auto-generated destructor stub
}

int ChatBox::createListenSocket(int listenPort, int listenQueue){
	int listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenFd < 0){
		perror("Bad socket");
		return listenFd;
	}

	sockaddr_in serverAddress;
	bzero((char*)&serverAddress, sizeof(sockaddr_in));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(listenPort);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	int bindTest = bind(listenFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if(bindTest < 0){
		perror("Bind to port failed");
		return bindTest;
	}

	int listenTest = listen(listenFd, listenQueue);
	if(listenTest <0){
		perror("Listen for incoming connections failed");
		return listenTest;
	}

	char chHostname[100];
	if(gethostname(chHostname, 100) < 0){
		perror("Failed to get hostname");
	}else{
		std::cout << "Listening on " << chHostname << ", " <<
				(gethostbyname(chHostname))->h_name << std::endl;
	}
	return listenFd;
}

/*
 * ChatBox.h
 *
 *  Created on: Jul 1, 2012
 *      Author: met898
 */

#include <string>

#ifndef CHATBOX_H_
#define CHATBOX_H_

class ChatBox {
	int port;
	std::string name;
	int maxClients;
public:
	ChatBox(int _port, std::string _name, int _maxClients);
	~ChatBox();

	static int createListenSocket(int listenPort, int listenQueue);
	void addClient(int newClient);
	void closeBox();
};

#endif /* CHATBOX_H_ */

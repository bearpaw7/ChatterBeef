/*
 * ChatBox.h
 *
 *  Created on: Jul 1, 2012
 *      Author: met898
 */

#include <string>
#include <vector>

#ifndef CHATBOX_H_
#define CHATBOX_H_

class ChatBox {

public:
	ChatBox(int _port, int _listenQueue, std::string _name, int _maxClients);
	~ChatBox();
	static int createListenSocket(int listenPort, int listenQueue);
	void closeBox();

private:
	std::vector<int> clients;
	int port;
	std::string name;
	int maxClients;
	void addClient(int newClient);

};

#endif /* CHATBOX_H_ */

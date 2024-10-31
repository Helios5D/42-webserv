#pragma once

#include "webserv.hpp"
#include "Server.hpp"
#include "Request.hpp"

class Server;
class Request;

class Client {
	private:
		int								_fd;
		std::map<std::string, Server*>	_servers;
		Request							*_request;
		bool							_response_ready;

	public:
		Client(int fd, std::map<std::string, Server*> servers);
		~Client();

		size_t							bytes_sent;
		bool							isResponseReady();
		void							setResponseReady(bool ready);
		void							setRequest(Request *request);
		int								getFd();
		std::map<std::string, Server*>	getServers();
		Request							*getRequest();
};
#pragma once

#include "webserv.hpp"
#include "Server.hpp"
#include "Request.hpp"

class Server;
class Request;

class Client {
	private:
		int		_fd;
		Server	*_server;
		Request	*_request;
		bool	_response_ready;
		
	public:
				Client(int fd, Server *server);
				~Client();
		size_t	bytes_sent;
		bool	isResponsReady();
		void	setResponseReady(bool ready);
		void	setRequest(Request *request);
		int		getFd();
		Server	*getServer();
		Request	*getRequest();
};
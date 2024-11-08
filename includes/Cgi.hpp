#pragma once

#include "webserv.hpp"
#include "Request.hpp"
#include "Client.hpp"

class Client;
class Request;

class Cgi {
	private:
		int		_fd;
		int		_pid;
		Client	*_client;
		Request	*_request;
		time_t	_start_time;
	public:
		Cgi(int fd, int pid, Client *client, time_t start_time);
		~Cgi();

		void	setRequest(Request *request);
		int		getFd();
		int		getPid();
		Client	*getClient();
		Request	*getRequest();
		time_t	getStartTime();
};
#pragma once

#include "webserv.hpp"
#include "Request.hpp"

class Request;

class Cgi {
	private:
		int		_fd;
		int		_pid;
		Request	*_request;
		time_t	_start_time;
	public:
		Cgi(int fd, int pid, Request *request, time_t start_time);
		~Cgi();
		
		void	setRequest(Request *request);
		int		getFd();
		int		getPid();
		Request	*getRequest();
		time_t	getStartTime();
};
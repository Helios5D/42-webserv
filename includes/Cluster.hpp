#pragma once

#include "Server.hpp"
#include "Request.hpp"

class Cluster {
	private:
		std::vector<Server>			_servers;
		std::vector<struct pollfd>	_poll_fds;
		std::map<int, Server*>		_client_to_server;
		std::map<int, Request*>		_client_responses;
		std::map<int, Request*>		_cgi_in;
		std::map<int, Request*>		_cgi_out;
		int							_epoll_fd;
	public:
				Cluster(t_cluster_config);
				~Cluster();
		bool	isServerFd(int fd);
		void	start();
		void	addToEpoll(int fd, __uint32_t events);
		void	displayServerInfo();
		void	handleClient(int fd);
		void	handleRequest(int fd);
		void	handleResponse(int fd);
		void	disconnectClient(int fd, bool error);
		void	setNonBlocking(int fd);
		void	executeCgi(Request &request);
		void	closeCluster();
};

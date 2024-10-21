#pragma once

#include "Server.hpp"

class Cluster {
	private:
		std::vector<Server>			_servers;
		std::vector<struct pollfd>	_poll_fds;
		std::map<int, Server*>		_client_to_server;
		int							_epoll_fd;
	public:
				Cluster(t_cluster_config);
				~Cluster();
		bool	isServerFd(int fd);
		void	start();
		void	displayServerInfo();
		void	handleClient(int fd);
		void	handleRequest(int fd);
		void	handleResponse(int fd);
		void	disconnectClient(int fd);
};

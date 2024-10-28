#pragma once

#include "Client.hpp"

class Request;
class Client;

class Cluster {
	private:
		std::vector<Server *>		_servers;
		std::vector<Client *>		_clients;
		std::vector<struct pollfd>	_poll_fds;
		std::map<int, Request*>		_cgi_out;
		int							_epoll_fd;
	public:
				Cluster(t_cluster_config);
				~Cluster();

		bool	isServerFd(int fd);
		bool	isCgiOut(int fd);
		
		ssize_t	findClient(int fd);

		void	start();
		void	closeCluster(bool print);

		void	addToEpoll(int fd, __uint32_t events);
		void	setNonBlocking(int fd);

		void	displayServerInfo();
		void	handleClient(int fd);
		void	handleRequest(int fd);
		void	handleResponse(int fd);
		void	disconnectClient(int fd, bool error);

		void	executeCgi(Request &request);
		char	**generateEnv(std::string body);
		void	readCgiOutput(int fd);
};

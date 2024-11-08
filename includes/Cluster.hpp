#pragma once

#include "Client.hpp"
#include "Cgi.hpp"
#include "Response.hpp"

class Request;
class Client;
class Cgi;
class Response;

class Cluster {
	private:
		std::vector<Server *>		_servers;
		std::vector<Client *>		_clients;
		std::vector<Cgi*>			_cgis;
		std::vector<struct pollfd>	_poll_fds;
		int							_epoll_fd;
	public:
				Cluster(t_cluster_config);
				~Cluster();

		bool	isServerFd(int fd);

		ssize_t	findClient(int fd);
		ssize_t	findCgi(int fd);

		void	start();
		void	closeCluster(bool print);
		void	closeFds();

		void	addToEpoll(int fd, __uint32_t events);
		void	deleteFromEpoll(int fd);
		void	modifyEvents(int fd, __uint32_t events);
		void	setNonBlocking(int fd);

		void	displayServerInfo();
		void	handleClient(int fd);
		void	handleRequest(int fd);
		void	handleResponse(int fd);
		void	disconnectClient(int fd);
		void	generateErrorResponse(Response &response, int code, std::string message, std::string warning);

		void	executeCgi(Client *client);
		char	**generateEnv(Client *client);
		void	freeEnv(char **env);
		void	readCgiOutput(int fd);
		int		checkCgiTimeout(Cgi *cgi);
};

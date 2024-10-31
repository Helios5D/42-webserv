#pragma once

#include "webserv.hpp"

class Server {
	private:
		int 						_socket_fd;
		struct sockaddr				_socket_addr;

		std::string					_ip;
		std::string					_port;
		std::string					_name;
		size_t						_client_max_body_size;
		std::map<int, std::string>	_error_pages;

		std::vector<t_location>		_locations;
		std::vector<struct pollfd>	_poll_fds;
	public:
		Server(t_server_config config);
		~Server();

		void								createSocket();
		void								listenSocket();
		int									acceptConnection();
		int									init();
		int									getFd() const;
		const std::string					&getIp() const ;
		const std::string					&getPort() const ;
		const std::string					&getName() const;
		const std::vector<t_location>		&getLocations() const;
		size_t								getClientMaxBodySize() const;
		const std::map<int, std::string>	&getErrorPages() const;
		Server 								&operator=(const Server &rhs);
		void								displayServerInfo();
		void								setNonBlocking(int fd);
};



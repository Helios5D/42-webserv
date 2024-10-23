/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 18:00:05 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/23 10:58:16 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(t_server_config config)
:	_name(config.server_name),
	_client_max_body_size(config.client_max_body_size),
	_error_pages(config.error_pages),
	_locations(config.locations)
{
	size_t colon = config.listen.find(':');
	if (colon != std::string::npos) {
		_ip = config.listen.substr(0, colon);
		_port = config.listen.substr(colon + 1);
	}
	else {
		_ip = "0.0.0.0";
		_port = config.listen;
	}
}

Server::~Server()
{}

void	Server::createSocket() {
	struct addrinfo hints, *res;

	std::memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (_ip == "0.0.0.0")
		hints.ai_flags = AI_PASSIVE;
	else
		hints.ai_flags = 0;

	int status;
	if ((status = getaddrinfo(_ip.c_str(), _port.c_str(), &hints, &res)) != 0) {
		throw std::runtime_error(std::string("getaddrinfo failed: ") + gai_strerror(status));
	}

	_socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (_socket_fd < 0) {
		freeaddrinfo(res);
		throw std::runtime_error("Server socket creation failed");
	}

	int optval = 1;
	if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
		freeaddrinfo(res);
		throw std::runtime_error("setsockopt failed");
	}

	if (bind(_socket_fd, res->ai_addr, res->ai_addrlen) < 0) {
		freeaddrinfo(res);
		throw std::runtime_error("Server socket binding failed");
	}
	_socket_addr = *res->ai_addr;
	freeaddrinfo(res);
	setNonBlocking(_socket_fd);
}

void	Server::setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) {
		throw std::runtime_error("Getting flags with fcntl failed");
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		throw std::runtime_error("Setting flags with fcntl failed");
	}
}

void	Server::listenSocket() {
	if (listen(_socket_fd, 10))
		throw std::runtime_error("Server socket listen failed");
}

int	Server::acceptConnection() {
	socklen_t addr_len = sizeof(_socket_addr);
	int connection_fd = accept(_socket_fd, &_socket_addr, &addr_len);
	if (connection_fd < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return -1;
		throw std::runtime_error("Server socket accept connection failed");
	}
	return connection_fd;
}

int Server::init() {

	createSocket();
	listenSocket();

	return _socket_fd;
}

int Server::getFd() const {
	return _socket_fd;
}

const std::string &Server::getPort() const {
	return _port;
}

const std::string &Server::getName() const {
	return _name;
}

const std::vector<t_location> &Server::getLocations() const {
	return _locations;
}

const std::map<int, std::string> &Server::getErrorPages() const {
	return _error_pages;
}

void Server::displayServerInfo() {
	std::cout << "     🌎 Server " << COL_CYAN << _name << COL_RESET;
	std::cout << " now listening on port " << COL_CYAN << _port << COL_RESET << std::endl;
}

Server  &Server::operator=(const Server &rhs) {
	if (this != &rhs) {
		this->_socket_fd = rhs._socket_fd;
		this->_socket_addr = rhs._socket_addr;
		this->_ip = rhs._ip;
		this->_port = rhs._port;
		this->_name = rhs._name;
		this->_client_max_body_size = rhs._client_max_body_size;
		this->_error_pages = rhs._error_pages;
		this->_locations = rhs._locations;
		this->_poll_fds = rhs._poll_fds;
	}
	return *this;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdaher <hdaher@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 18:00:05 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/18 16:00:54 by hdaher           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(t_server_config config)
:	_listen(atoi(config.listen.substr(config.listen.size() - 4).c_str())),
	_name(config.server_name), _client_max_body_size(config.client_max_body_size), _error_pages(config.error_pages)
{}

Server::~Server()
{}

void	Server::createSocket() {
	_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (_socket_fd < 0)
		throw std::runtime_error("Server socket creation failed");
	_socket_addr.sin_family = AF_UNIX;
	_socket_addr.sin_addr.s_addr = INADDR_ANY;
	_socket_addr.sin_port = htons(_listen);
}

void	Server::bindSocket() {
	if (bind(_socket_fd, (struct sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
		throw std::runtime_error("Server socket binding failed");
}

void	Server::listenSocket() {
	if (listen(_socket_fd, 10))
		throw std::runtime_error("Server socket listen failed");
}

int	Server::acceptConnection() {
	int connection_fd = accept(_socket_fd, (struct sockaddr *)&_socket_addr, (socklen_t *)sizeof(_socket_addr));
	if (connection_fd < 0)
		throw std::runtime_error("Server socket accept connection failed");
	return connection_fd;
}

int Server::init() {

	createSocket();
	bindSocket();
	listenSocket();

	return _socket_fd;
}

int Server::getFd() {
	return _socket_fd;
}

int Server::getListen() {
	return _listen;
}

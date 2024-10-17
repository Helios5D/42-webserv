/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 14:54:44 by hdaher            #+#    #+#             */
/*   Updated: 2024/10/17 15:31:20 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"

ServerSocket::ServerSocket(int port) {
	_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (_socket_fd < 0)
		throw std::runtime_error("Server socket creation failed");
	_socket_addr.sin_family = AF_UNIX;
	_socket_addr.sin_addr.s_addr = INADDR_ANY;
	_socket_addr.sin_port = htons(port);
}

ServerSocket::~ServerSocket() {
	close(_socket_fd);
}

void	ServerSocket::bindSocket() {
	if (bind(_socket_fd, (struct sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
		throw std::runtime_error("Server socket binding failed");
}

void	ServerSocket::listenSocket() {
	if (listen(_socket_fd, 10))
		throw std::runtime_error("Server socket listen failed");
}

int	ServerSocket::acceptConnection() {
	int connection_fd = accept(_socket_fd, (struct sockaddr *)&_socket_addr, (socklen_t *)sizeof(_socket_addr));
	if (connection_fd < 0)
		throw std::runtime_error("Server socket accept connection failed");
	return connection_fd;
}

int ServerSocket::getFd() {
	return _socket_fd;
}

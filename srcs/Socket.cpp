/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdaher <hdaher@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 14:54:44 by hdaher            #+#    #+#             */
/*   Updated: 2024/10/16 15:29:44 by hdaher           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(int port) {
	_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (_socket_fd < 0)
		throw std::exception();
	_socket_addr.sin_family = AF_UNIX;
	_socket_addr.sin_addr.s_addr = INADDR_ANY;
	_socket_addr.sin_port = htons(port);
}

Socket::~Socket() {
	close(_socket_fd);
}

void	Socket::bindSocket() {
	if (bind(_socket_fd, (struct sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
		throw std::exception();
}

void	Socket::listenSocket() {
	if (listen(_socket_fd, 10))
		throw std::exception();
}

int	Socket::acceptConnection() {
	int connection_fd = accept(_socket_fd, (struct sockaddr *)&_socket_addr, (socklen_t *)sizeof(_socket_addr));
	if (connection_fd < 0)
		throw std::exception();
	return connection_fd;
}

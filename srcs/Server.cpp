/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 18:00:05 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/17 18:00:06 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool running = false;

void handleSignal(int signal) {
	(void)signal;
	running = false;
}

Server::Server(t_server_config config)
: _listen(atoi(config.listen.c_str())), _name(config.server_name), _client_max_body_size(config.client_max_body_size), _error_pages(config.error_pages), _server_socket(ServerSocket(_listen))
{}

Server::~Server()
{}

void Server::init() {
	_server_socket.bindSocket();
	_server_socket.listenSocket();

	struct pollfd server_poll;
	server_poll.fd = _server_socket.getFd();
	server_poll.events = POLLIN;

	_poll_fds.push_back(server_poll);
}

void Server::start() {

	std::cout << "Launching server..." << std::endl;
	init();
	std::cout << "Listening on port : " << COL_CYAN << _listen << COL_RESET << std::endl;

	running = true;
	signal(SIGINT, handleSignal);
	while (running)
	{
		if (poll(_poll_fds.data(), _poll_fds.size(), -1) < 0)
		{
			if (!running)
				break ;
			throw std::runtime_error("Poll failed");
		}

		for (size_t i = 0; i < _poll_fds.size(); i++) {
			if (_poll_fds[i].revents) {
				if (_poll_fds[i].fd == _server_socket.getFd()) {
					clientConnect();
				}
				else {
					std::string request = readRequest(_poll_fds[i].fd);
					if (request.empty()) {
						clientDisconnect(i);
						i--;
					}
					else
						handleRequest(request);
				}
			}
		}
	}

	std::cout << COL_BLUE << "Closing server ..." << COL_RESET << std::endl;
	close(_server_socket.getFd());

	for (size_t i = 1; i < _poll_fds.size(); i++)
		close(_poll_fds[i].fd);
}

void Server::clientConnect() {
	int client_fd = _server_socket.acceptConnection();
	struct pollfd client_poll;
	client_poll.fd = client_fd;
	client_poll.events = POLLIN;
	_poll_fds.push_back(client_poll);
}

void Server::clientDisconnect(int index) {
	close(_poll_fds[index].fd);
	_poll_fds.erase(_poll_fds.begin() + index);
}

void Server::handleRequest(std::string request) {
	(void)request;
}

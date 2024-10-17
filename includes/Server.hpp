/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 18:00:15 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/17 18:00:16 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "ServerSocket.hpp"

class Server {
	private:
		int							_listen;
		std::string					_name;
		std::string					_client_max_body_size;
		std::map<int, std::string>	_error_pages;

		ServerSocket				_server_socket;
		std::vector<struct pollfd> _poll_fds;
	public:
				Server(int port);
				~Server();

		void	init();
		void	start();
		void	clientConnect();
		void	clientDisconnect(int index);
		void	handleRequest(std::string request);
};



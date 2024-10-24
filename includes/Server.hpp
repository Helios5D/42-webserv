/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 18:00:15 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/22 17:42:30 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class Server {
	private:
		int 						_socket_fd;
		struct sockaddr				_socket_addr;

		std::string					_ip;
		std::string					_port;
		std::string					_name;
		std::string					_client_max_body_size;
		std::map<int, std::string>	_error_pages;

		std::vector<t_location>		_locations;
		std::vector<struct pollfd>	_poll_fds;
	public:
		Server(t_server_config config);
		~Server();

		void							createSocket();
		void							listenSocket();
		int								acceptConnection();
		int								init();
		int								getFd() const;
		const std::string				&getPort() const ;
		const std::string				&getName() const;
		const std::vector<t_location>	&getLocations() const;
		Server 							&operator=(const Server &rhs);
		void							displayServerInfo();
		void							setNonBlocking(int fd);
};



/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdaher <hdaher@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 18:00:15 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/18 15:17:38 by hdaher           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Config.hpp"

class Server {
	private:
		int 						_socket_fd;
		sockaddr_in					_socket_addr;

		int							_listen;
		std::string					_name;
		std::string					_client_max_body_size;
		std::map<int, std::string>	_error_pages;

		std::vector<struct pollfd> _poll_fds;
	public:
				Server(t_server_config config);
				~Server();

		void	createSocket();
		void	bindSocket();
		void	listenSocket();
		int		acceptConnection();
		int		init();
		int		getFd();
};



/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 14:49:02 by hdaher            #+#    #+#             */
/*   Updated: 2024/10/17 17:31:46 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class ServerSocket {
	private:
		int 		_socket_fd;
		sockaddr_in	_socket_addr;
	public:
				ServerSocket(int port);
				~ServerSocket();
		void	bindSocket();
		void	listenSocket();
		int		acceptConnection();
		int		getFd();
};

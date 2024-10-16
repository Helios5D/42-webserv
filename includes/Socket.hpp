/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 14:49:02 by hdaher            #+#    #+#             */
/*   Updated: 2024/10/16 16:10:35 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

class Socket {
	private:
		int 		_socket_fd;
		sockaddr_in	_socket_addr;
	public:
				Socket(int port);
				~Socket();
		void	bindSocket();
		void	listenSocket();
		int		acceptConnection();
};

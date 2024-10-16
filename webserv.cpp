/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdaher <hdaher@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 11:52:34 by hdaher            #+#    #+#             */
/*   Updated: 2024/10/16 16:17:19 by hdaher           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Socket.hpp"

int main(int argc, char **argv) {
	std::string	conf_path;
	if (argc > 2) {
		std::cerr << "Error: Wrong numbers of arguments" << std::endl;
		return 1;
	}
	else if (argc == 1)
		conf_path = "./webserv.conf";
	else
		conf_path = argv[1];

	Socket socket(8080);
	socket.bindSocket();
	socket.listenSocket();
	while (1)
	{
		int connection_fd = socket.acceptConnection();
		std::string request = readRequest(connection_fd);
	}

	return 0;
}

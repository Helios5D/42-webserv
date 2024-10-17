/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 11:52:34 by hdaher            #+#    #+#             */
/*   Updated: 2024/10/17 15:04:37 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Server.hpp"

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

	try {
		Server server(8080);
		server.start();
	}
	catch (std::exception &e) {
		std::cerr << COL_RED << e.what() << COL_RESET << std::endl;
	}

	return 0;
}

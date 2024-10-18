/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdaher <hdaher@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 11:52:34 by hdaher            #+#    #+#             */
/*   Updated: 2024/10/18 11:08:14 by hdaher           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Cluster.hpp"

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

	t_cluster_config config;
	try {
		config = parseConfigFile(conf_path);
		printClusterConfig(config);
	}
	catch (std::exception &e) {
		std::cerr << COL_RED << e.what() << COL_RESET << std::endl;
	}

	Cluster cluster(config);

	return 0;
}

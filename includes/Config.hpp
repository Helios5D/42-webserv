/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 18:00:21 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/17 18:00:23 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

typedef struct s_location {
	std::string					path;
	std::string					root;
	std::string					index;
	std::string					cgi_extension;
	std::string					upload_save;
	std::vector<std::string>	allowed_methods;
} t_location;

typedef struct s_server_config {
	std::string						listen;
	std::string						server_name;
	std::string						client_max_body_size;
	std::map<int, std::string>		error_pages;
	std::vector<t_location>	locations;
} t_server_config;

typedef struct s_cluster_config {
	std::vector<t_server_config>	servers;
} t_cluster_config;

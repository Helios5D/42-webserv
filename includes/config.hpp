#pragma once

#include "webserv.hpp"

typedef struct s_location {
	std::string					path;
	std::string					root;
	std::string					index;
	std::string					cgi_extension;
	std::string					upload_save;
	std::string					autoindex;
	std::vector<std::string>	allowed_methods;
} t_location;

typedef struct s_server_config {
	std::string						ip;
	std::string						port;
	std::string						server_name;
	long							client_max_body_size;
	std::map<int, std::string>		error_pages;
	std::vector<t_location>			locations;
} t_server_config;

typedef struct s_cluster_config {
	std::vector<t_server_config>	servers;
} t_cluster_config;

t_cluster_config	parseConfigFile(std::string path);
void				printClusterConfig(const t_cluster_config& cluster);

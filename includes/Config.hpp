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
	std::string					redir_path;
	int							redir_code;
} t_location;

typedef struct s_server_config {
	std::string						error;
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

class Config {
	private:
		Config();
		~Config();
	public:
		static t_cluster_config	parseConfigFile(std::string path);
		static void				printClusterConfig(const t_cluster_config& cluster);
		static t_server_config	parseServerBlock(std::stringstream &ss);
		static t_location		parseLocationBlock(std::stringstream &ss);
		static void				fillEmptyFields(t_cluster_config &cluster);
};

std::ostream &operator<<(std::ostream &os, const t_cluster_config &cluster);

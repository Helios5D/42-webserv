#pragma once

#include "webserv.hpp"

struct Location {
	std::string					path;
	std::string					root;
	std::string					index;
	std::string					cgi_extension;
	std::string					upload_save;
	std::vector<std::string>	allowed_methods;
};

struct ServerConfig {
	std::string						listen;
	std::string						server_name;
	std::string						client_max_body_size;
	std::map<int, std::string>		error_pages;
	std::vector<struct Location>	locations;
};

struct	ClusterConfig {
	std::vector<struct ServerConfig>		servers;
};

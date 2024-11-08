#include "Config.hpp"

Config::Config() {}
Config::~Config() {}

void Config::fillEmptyFields(t_cluster_config &cluster) {
	for (std::vector<t_server_config>::iterator it = cluster.servers.begin(); it != cluster.servers.end(); ++it) {
		t_server_config&	server = *it;
		if (server.port.empty())
			throw std::invalid_argument("Server listen field must be set");

		if (server.server_name.empty())
			server.server_name = "webserv.com";
		if (server.client_max_body_size == -1)
			server.client_max_body_size = 1073741824;
		if (server.error_pages.find(400) == server.error_pages.end())
			server.error_pages[400] = "./web/errors/400.html";
		if (server.error_pages.find(403) == server.error_pages.end())
			server.error_pages[403] = "./web/errors/403.html";
		if (server.error_pages.find(404) == server.error_pages.end())
			server.error_pages[404] = "./web/errors/404.html";
		if (server.error_pages.find(405) == server.error_pages.end())
			server.error_pages[405] = "./web/errors/405.html";
		if (server.error_pages.find(413) == server.error_pages.end())
			server.error_pages[413] = "./web/errors/413.html";
		if (server.error_pages.find(423) == server.error_pages.end())
			server.error_pages[423] = "./web/errors/423.html";
		if (server.error_pages.find(500) == server.error_pages.end())
			server.error_pages[500] = "./web/errors/500.html";
		if (server.error_pages.find(502) == server.error_pages.end())
			server.error_pages[502] = "./web/errors/502.html";
		if (server.error_pages.find(504) == server.error_pages.end())
			server.error_pages[504] = "./web/errors/504.html";
		if (server.error_pages.find(505) == server.error_pages.end())
			server.error_pages[505] = "./web/errors/505.html";
		if (server.locations.empty())
		{
			t_location location;
			location.path = "/";
			location.root = "/pages";
			location.index = "index.html";
			location.allowed_methods.push_back("GET");
			location.allowed_methods.push_back("POST");
			location.allowed_methods.push_back("DELETE");
			location.cgi_extension = ".py";
			location.upload_save = "/uploads/";
			location.autoindex = "off";
		} else {
			for (std::vector<t_location>::iterator ite = server.locations.begin(); ite != server.locations.end(); ++ite) {
				t_location &location = *ite;
				if (location.path.empty())
					throw std::invalid_argument("Location path must be set");
				if (location.root.empty())
					throw std::invalid_argument("Location root must be set");
				if (location.index.empty())
					location.index = "index.html";
				if (location.allowed_methods.empty()) {
					location.allowed_methods.push_back("GET");
					location.allowed_methods.push_back("POST");
					location.allowed_methods.push_back("DELETE");
				}
				if (location.cgi_extension.empty())
					location.cgi_extension = ".py";
				if (location.upload_save.empty())
					location.upload_save = "/uploads/";
				if (location.autoindex.empty())
					location.autoindex = "on";

			}
		}
	}
}

t_location Config::parseLocationBlock(std::stringstream &ss) {
	t_location	location;
	std::string	line;

	location.redir_code = -1;

	while (std::getline(ss, line)) {
		trim(line);
		if (line.empty())
			continue;

		else if (line == "}")
			return location;
		else if (line[line.size() - 1] != ';')
			throw std::invalid_argument("Parsing error at: " + line);
		else if (line.find("root ") == 0) {
			if (!location.root.empty())
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);

			line = line.substr(5, line.size() - 6);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");

			location.root = line;
		} else if (line.find("index ") == 0) {
			if (!location.index.empty())
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);

			line = line.substr(6, line.size() - 7);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");

			location.index = line;
		} else if (line.find("cgi_extension ") == 0) {
			if (!location.cgi_extension.empty())

				throw std::invalid_argument("Parsing error: Duplicate at: " + line);
			line = line.substr(14, line.size() - 15);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");
			if (line != ".py" && line != ".php")
				throw std::invalid_argument("Parsing error: Invalid CGI extension: " + line);

			location.cgi_extension = line;
		} else if (line.find("upload_save ") == 0) {
			if (!location.upload_save.empty())
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);

			line = line.substr(12, line.size() - 13);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");

			location.cgi_extension = line;
		} else if (line.find("autoindex ") == 0) {
			if (!location.autoindex.empty())

				throw std::invalid_argument("Parsing error: Duplicate at: " + line);
			line = line.substr(10, line.size() - 11);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");
			if (line != "on" && line != "off")
				throw std::invalid_argument("Parsing error: Invalid value for autoindex: " + line);

			location.autoindex = line;
		} else if (line.find("return ") == 0) {
			if (location.redir_code != -1)
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);

			line = line.substr(7, line.size() - 8);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");

			std::istringstream iss(line);
			std::string code;
			getline(iss, code, ' ');
			if (!isNumber(code) || code.size() != 3)
				throw std::invalid_argument("Parsing error: Wrong return code: " + code);
			location.redir_code = std::atoi(code.c_str());
			getline(iss, location.redir_path);
		} else if (line.find("allowed_methods ") == 0) {
			if (!location.allowed_methods.empty())
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);

			line = line.substr(16, line.size() - 17);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");

			std::istringstream iss(line);
			std::string method;
			while (std::getline(iss, method, ' ')) {
				if (method != "GET" && method != "POST" && method != "DELETE")
					throw std::invalid_argument("Parsing error: Invalid method: " + method);
				location.allowed_methods.push_back(method);
			}
		} else
			throw std::invalid_argument("Parsing error at: " + line);
	}
	throw std::invalid_argument("Parsing error at: " + line);
	return location;
}

t_server_config Config::parseServerBlock(std::stringstream &ss) {
	t_server_config	server_config;
	server_config.client_max_body_size = -1;

	std::string line;
	while (std::getline(ss, line)) {
		trim(line);
		if (line.empty())
			continue;

		if (line.find("location ") == 0) {
			line = line.substr(9);
			trim(line);

			std::istringstream iss(line);
			std::string path;
			std::getline(iss, path, ' ');
			std::string a;
			std::getline(iss, a, ' ');
			if (a != "{")
				throw std::invalid_argument("Parsing error at: " + line);

			t_location location = parseLocationBlock(ss);
			location.path = path;
			server_config.locations.push_back(location);
		} else if (line == "}")
			return server_config;
		else if (line[line.size() - 1] != ';')
			throw std::invalid_argument("Parsing error at: " + line);
		else if (line.find("listen ") == 0) {
			if (!server_config.port.empty())
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);

			line = line.substr(7, line.size() - 8);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");

			size_t colon = line.find(':');
			if (colon != std::string::npos) {
				server_config.ip = line.substr(0, colon);
				server_config.port = line.substr(colon + 1);
				if (!isNumber(server_config.port))
					throw std::invalid_argument("Parsing error: Invalid port: " + server_config.port);
			} else {
				server_config.ip = "0.0.0.0";
				server_config.port = line;
			}
		} else if (line.find("server_name ") == 0) {
			if (!server_config.server_name.empty())
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);

			line = line.substr(12, line.size() - 13);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");

			server_config.server_name = line;
		} else if (line.find("client_max_body_size ") == 0) {
			if (server_config.client_max_body_size != -1)
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);

			line = line.substr(21, line.size() - 22);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");

			if (!isNumber(line))
				throw std::invalid_argument("Parsing error: Invalid client max body size value at: " + line);

			server_config.client_max_body_size = std::atol(line.c_str());
		} else if (line.find("error_page ") == 0) {
			line = line.substr(11, line.size() - 12);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");

			if (!isNumber(line.substr(0, 3)))
				throw std::invalid_argument("Parsing error at: " + line);

			server_config.error_pages[std::atoi(line.substr(0, 3).c_str())] = line.substr(4, line.size() - 4);

		} else
			throw std::invalid_argument("Parsing error at: " + line);
	}

	throw std::invalid_argument("Parsing error at: " + line);
	return server_config;
}

t_cluster_config Config::parseConfigFile(std::string path) {
	std::ifstream conf_file(path.c_str());
	std::stringstream ss;
	t_cluster_config cluster_config;
	std::string line;

	if (!conf_file.is_open())
		throw std::runtime_error("Could not open config file");
	ss << conf_file.rdbuf();
	conf_file.close();

	while (std::getline(ss, line)) {
		trim(line);
		if (line.empty())
			continue;

		if (line.find("server ") == 0) {
			line = line.substr(6);
			trim(line);
			if (line != "{")
				throw std::invalid_argument("Parsing error at: " + line);
			t_server_config server_config = parseServerBlock(ss);

			for (size_t i = 0; i < cluster_config.servers.size(); i++) {
				if (cluster_config.servers[i].port == server_config.port)
					throw std::invalid_argument("Parsing error: invalid ports");
			}
			cluster_config.servers.push_back(server_config);
		}
		else
			throw std::invalid_argument("Parsing error at: " + line);
	}
	try {
		fillEmptyFields(cluster_config);
	}
	catch (const std::exception &e) {
		throw std::runtime_error(std::string("Parsing error: ") + e.what());
	}
	return cluster_config;
}

#include <iostream>

std::ostream &operator<<(std::ostream &os, const t_cluster_config &cluster) {
	for (std::vector<t_server_config>::const_iterator it = cluster.servers.begin(); it != cluster.servers.end(); ++it) {
		const t_server_config &server = *it;

		os << "Server " << (it - cluster.servers.begin() + 1) << ":\n";
		os << "  Port: " << server.port << std::endl;
		os << "  IP: " << server.ip << std::endl;
		os << "  Server Name: " << server.server_name << std::endl;
		os << "  Client Max Body Size: " << server.client_max_body_size << std::endl;

		os << "  Error Pages" << std::endl;
		for (std::map<int, std::string>::const_iterator err_it = server.error_pages.begin(); err_it != server.error_pages.end(); ++err_it) {
			os << "    Error Code " << err_it->first << ": " << err_it->second << std::endl;
		}

		os << "  Locations" << std::endl;
		for (std::vector<t_location>::const_iterator loc_it = server.locations.begin(); loc_it != server.locations.end(); ++loc_it) {
			const t_location &location = *loc_it;
			os << "    Location " << (loc_it - server.locations.begin() + 1) << std::endl;
			os << "      Path: " << location.path << std::endl;
			os << "      Root: " << location.root << std::endl;
			os << "      Index: " << location.index << std::endl;
			os << "      CGI Extension: " << location.cgi_extension << std::endl;
			os << "      Upload Save: " << location.upload_save << std::endl;
			os << "      Redir path: " << location.redir_path << std::endl;
			os << "      Redir code: " << location.redir_code << std::endl;

			os << "      Allowed Methods: ";
			for (std::vector<std::string>::const_iterator method_it = location.allowed_methods.begin(); method_it != location.allowed_methods.end(); ++method_it) {
				os << *method_it;
				if (method_it + 1 != location.allowed_methods.end()) {
					os << ", ";
				}
			}
			os << std::endl;
		}
		os << std::endl;
	}
	return os;
}


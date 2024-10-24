#include "webserv.hpp"

void fillEmptyFields(t_cluster_config &cluster) {
	for (std::vector<t_server_config>::iterator it = cluster.servers.begin(); it != cluster.servers.end(); ++it) {
		t_server_config& server = *it;
		if (server.port.empty())
			throw std::invalid_argument("Server listen field must be set");
		if (server.server_name.empty())
			server.server_name = "webserv.com";
		if (server.client_max_body_size == -1)
			server.client_max_body_size = 1073741824;
		if (server.error_pages.find(404) == server.error_pages.end())
			server.error_pages[404] = "/pages/404.html";
		if (server.error_pages.find(505) == server.error_pages.end())
			server.error_pages[505] = "/pages/505.html";
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
		}
		else {
			for (std::vector<t_location>::iterator ite = server.locations.begin(); ite != server.locations.end(); ++ite) {
				t_location &location = *ite;
				if (location.path.empty())
					throw std::invalid_argument("Location path must be set");
				if (location.root.empty())
					throw std::invalid_argument("Location root must be set");
			}
		}
	}
}

void printClusterConfig(const t_cluster_config &cluster) {
	for (std::vector<t_server_config>::const_iterator it = cluster.servers.begin(); it != cluster.servers.end(); ++it) {
		const t_server_config& server = *it;
		std::cout << "Server " << (it - cluster.servers.begin() + 1) << ":\n";
		std::cout << "  Port: " << server.port << "\n";
		std::cout << "  Ip: " << server.ip << "\n";
		std::cout << "  Server Name: " << server.server_name << "\n";
		std::cout << "  Client Max Body Size: " << server.client_max_body_size << "\n";

		// Print error pages
		std::cout << "  Error Pages:\n";
		for (std::map<int, std::string>::const_iterator err_it = server.error_pages.begin(); err_it != server.error_pages.end(); ++err_it) {
			std::cout << "    Error Code " << err_it->first << ": " << err_it->second << "\n";
		}

		// Print locations
		std::cout << "  Locations:\n";
		for (std::vector<t_location>::const_iterator loc_it = server.locations.begin(); loc_it != server.locations.end(); ++loc_it) {
			const t_location& location = *loc_it;
			std::cout << "    Location " << (loc_it - server.locations.begin() + 1) << ":\n";
			std::cout << "      Path: " << location.path << "\n";
			std::cout << "      Root: " << location.root << "\n";
			std::cout << "      Index: " << location.index << "\n";
			std::cout << "      CGI Extension: " << location.cgi_extension << "\n";
			std::cout << "      Upload Save: " << location.upload_save << "\n";

			// Print allowed methods
			std::cout << "      Allowed Methods: ";
			for (std::vector<std::string>::const_iterator method_it = location.allowed_methods.begin(); method_it != location.allowed_methods.end(); ++method_it) {
				std::cout << *method_it;
				if (method_it + 1 != location.allowed_methods.end()) {
					std::cout << ", ";
				}
			}
			std::cout << "\n";
		}
	}
}

t_location parseLocationBlock(std::stringstream &ss) {
	t_location location;
	std::string line;
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
		}
		else if (line.find("index ") == 0) {
			if (!location.index.empty())
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);
			line = line.substr(6, line.size() - 7);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");
			location.index = line;
		}
		else if (line.find("cgi_extension ") == 0) {
			if (!location.cgi_extension.empty())
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);
			line = line.substr(14, line.size() - 15);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");
			if (line != ".py" && line != ".php")
				throw std::invalid_argument("Parsing error: Invalid CGI extension: " + line);
			location.cgi_extension = line;
		}
		else if (line.find("upload_save ") == 0) {
			if (!location.upload_save.empty())
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);
			line = line.substr(12, line.size() - 13);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");
			location.cgi_extension = line;
		}
		else if (line.find("autoindex ") == 0) {
			if (!location.autoindex.empty())
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);
			line = line.substr(10, line.size() - 11);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");
			if (line != "on" && line != "off")
				throw std::invalid_argument("Parsing error: Invalid value for autoindex: " + line);
			location.autoindex = line;
		}
		else if (line.find("allowed_methods ") == 0) {
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
		}
		else
			throw std::invalid_argument("Parsing error at: " + line);
	}
	throw std::invalid_argument("Parsing error at: " + line);
	return location;
}

t_server_config parseServerBlock(std::stringstream &ss) {
	t_server_config server_config;
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
		}
		else if (line == "}")
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
			}
			else {
				server_config.ip = "0.0.0.0";
				server_config.port = line;
			}
		}
		else if (line.find("server_name ") == 0) {
			if (!server_config.server_name.empty())
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);
			line = line.substr(12, line.size() - 13);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");
			server_config.server_name = line;
		}
		else if (line.find("client_max_body_size ") == 0) {
			if (server_config.client_max_body_size != -1)
				throw std::invalid_argument("Parsing error: Duplicate at: " + line);
			line = line.substr(21, line.size() - 22);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");
			if (!isNumber(line))
				throw std::invalid_argument("Parsing error: Invalid client max body size value at: " + line);
			server_config.client_max_body_size = std::atol(line.c_str());
		}
		else if (line.find("error_page ") == 0) {
			line = line.substr(11, line.size() - 12);
			if (line.empty())
				throw std::invalid_argument("Parsing error: Empty value");
			if (!isNumber(line.substr(0, 3)))
				throw std::invalid_argument("Parsing error at: " + line);
			server_config.error_pages[std::atoi(line.substr(0, 3).c_str())] = line.substr(4, line.size() - 4);
		}
		else
			throw std::invalid_argument("Parsing error at: " + line);
	}
	throw std::invalid_argument("Parsing error at: " + line);
	return server_config;
}

t_cluster_config parseConfigFile(std::string path) {
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
			cluster_config.servers.push_back(parseServerBlock(ss));
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


#include "webserv.hpp"
#include "Config.hpp"

t_location parseLocationBlock(std::stringstream &ss) {
	t_location location;
	std::string line;
	while (std::getline(ss, line)) {
		trim(line);
		if (line.empty())
			continue;
		else if (line == "}")
			return location;
		else if (line.back() != ';')
			throw std::invalid_argument("Parsing error");

		else if (find("root") == 0) {
			line = line.substr(4, line.size() - 5);
			location.root = line;
		}
		else if (find("index") == 0) {
			line = line.substr(5, line.size() - 6);
			location.index = line;
		}
		// Add handling of other keys such as cgi_extension or upload_save
	}
	throw std::invalid_argument("Parsing error");
	return location;
}

t_server_config parseServerBlock(std::stringstream &ss) {
	t_server_config server_config;
	std::string line;
	while (std::getline(ss, line)) {
		trim(line);
		if (line.empty())
			continue;
		if (line.find("location") == 0) {
			line = line.substr(6);
			trim(line);
			if (line != "{")
				throw std::invalid_argument("Parsing error");
			server_config.locations.push_back(parseLocationBlock(ss));
		}
		else if (line == "}")
			return server_config;
		else if (line.back() != ';')
			throw std::invalid_argument("Parsing error");

		else if (find("listen") == 0) {
			line = line.substr(6, line.size() - 7);
			server_config.listen = line;
		}
		else if (find("server_name") == 0) {
			line = line.substr(11, line.size() - 12);
			server_config.server_name = line;
		}
		else if (find("client_max_body_size") == 0) {
			line = line.substr(20, line.size() - 21);
			server_config.server_name = line;
		}
		else if (find("error_page") == 0) {
			line = line.substr(10, line.size());
			for (size_t i = 0; i < 3; i++) {
				if (!std::isdigit(line[i]))
					throw std::invalid_argument("Parsing error");
			}
			server_config.error_pages[atoi(line.substr(0, 3))] = line.substr(4, line.size() - 5);
		}
	}
	throw std::invalid_argument("Parsing error");
	return server_config;
}

t_cluster_config parseConfigFile(std::string path) {
	std::ifstream conf_file(path);
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
		if (line.find("server") == 0) {
			line = line.substr(6);
			trim(line);
			if (line != "{")
				throw std::invalid_argument("Parsing error");
			cluster_config.servers.push_back(parseServerBlock(ss));
		}
	}
	return cluster_config;
}


#include "webserv.hpp"
#include "Config.hpp"

struct ClusterConfig parseConfigFile(std::string path) {
	std::ifstream config_file(path);
	ClusterConfig config;
	std::string line;

	if (!config_file.is_open())
		throw std::runtime_error("Could not open config file");

	while (std::getline(config_file, line)) {
		trim(line);
		if (line.empty())
			continue;
		if (line.find("server") == 0) {
			line = line.substr(6);
			trim(line)
			if (line != "{")
				throw std::invalid_argument("Parsing error");
			parseServerBlock(config_file);
		}
	}
}

void parseLocationBlock(std::ifstream file_stream) {
	while (std::getline(configFile, line)) {
		trim(line);
		if (line.empty())
			continue;
		else if (line == "}")
			return ;
		else if (line == '')
	}
	throw std::invalid_argument("Parsing Error");
}

void parseServerBlock(std::ifstream file_stream) {
	while (std::getline(configFile, line)) {
		trim(line);
		if (line.empty())
			continue;
		if (line.find("location") == 0) {
			line = line.substr(6);
			trim(line)
			if (line != "{")
				throw std::invalid_argument("Parsing error");
			parseLocationBlock(config_file);
		}



		else if (line == "}")
			return ;
		else if (line == '')
	}
	throw std::invalid_argument("Parsing Error");
}

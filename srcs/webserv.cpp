#include "webserv.hpp"
#include "Cluster.hpp"

int main(int argc, char **argv) {
	std::string	conf_path;
	if (argc > 2) {
		std::cerr << "Error: Wrong numbers of arguments" << std::endl;
		return 1;
	} else if (argc == 1)
		conf_path = "./webserv.conf";
	else
		conf_path = argv[1];

	t_cluster_config config;
	try {
		config = Config::parseConfigFile(conf_path);
		// std::cout << config;
	} catch (std::exception &e) {
		std::cerr << COL_RED << e.what() << COL_RESET << std::endl;
		return 1;
	}

	initializeCodesResponses();
	initializeContentTypes();

	Cluster cluster(config);

	try {
		cluster.start();
	} catch (std::runtime_error &e) {
		std::cerr << COL_RED << std::string("Error: ") + e.what() << COL_RESET << std::endl;
		cluster.closeCluster(true);
		return 1;
	} catch (std::exception &e) {
		cluster.closeCluster(false);
		return 1;
	}

	return 0;
}

// #include "Request.hpp"

// int main(void) {
// 	// std::ofstream file("fichier.txt");
// 	// file << "GET / HTTP/1.1\r\nContent-Type: application/json\r\nUser-Agent: PostmanRuntime/7.28.3\r\nAccept: */*\r\nCache-Control: no-Cache\r\nPostman-Token: 8328391905\r\nHost:jsonplaceholder.typicode.com\r\nAccept-Encoding: gzip,deflate,br\r\nConnection: keep-alive\r\nContent-Length: 56\r\n\r\n{\r\n  \"title\":\"foo\",\r\n  \"body\":\"bar\",\r\n  \"userId\":1\r\n}ubuvbgvgvgvugvgvugvuvuvbuvbyuvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\r\n";
// 	// file.close();
// 	// return 0;

// 	t_cluster_config config = parseConfigFile("./webserv.conf");
// 	Server server = config.servers[0];

// 	int fd = open("fichier.txt", O_RDONLY);
// 	if (fd < 0) {
// 		std::cerr << "ERROR" << std::endl;
// 		return 1;
// 	}
// 	Request request(fd, server);

// 	std::cout << "Method: " << request.getMethod() << std::endl;
// 	std::cout << "Target file: " << request.getTargetFile() << std::endl;
// 	std::cout << "Headers: " << std::endl;

// 	std::map<std::string, std::string> headers = request.getHeaders();
// 	std::map<std::string, std::string>::const_iterator it = headers.begin();
// 	std::map<std::string, std::string>::const_iterator end = headers.end();

// 	for (; it != end; it++)
// 		std::cout << "\t" << (*it).first << ":" << (*it).second << std::endl;

// 	std::cout << "Body: " << request.getBody() << std::endl;
// 	std::cout << "Response code: " << request.getResCode() << std::endl;
// 	std::cout << "Response: " << request.getResponse() << std::endl;

// 	return 0;
// }

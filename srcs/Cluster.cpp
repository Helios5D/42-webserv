#include "webserv.hpp"
#include "Request.hpp"
#include "Cluster.hpp"

bool running = false;

void handleSignal(int signal) {
	(void)signal;
	running = false;
}

Cluster::Cluster(t_cluster_config config)
{
	std::cout << std::endl << COL_BLUE << "==============================" << std::endl;
	std::cout << "    🌐 Webserv Cluster 🌐    " << std::endl;
	std::cout << "==============================" << COL_RESET << std::endl << std::endl;
	std::cout << " 🔵 [INFO] Initializing server" << std::endl;

	_epoll_fd = epoll_create1(0);
	if (_epoll_fd < 0)
		throw std::runtime_error("Epoll creation failed");
	for (size_t i = 0; i < config.servers.size(); i++) {
		Server *server = new Server(config.servers[i]);
		_servers.push_back(server);
	}
}

Cluster::~Cluster()
{}

void Cluster::displayServerInfo() {
	std::cout << " 🔵 [INFO] Cluster Info" << std::endl;
	for (size_t i = 0; i < _servers.size(); i++) {
		_servers[i]->displayServerInfo();
	}
	std::cout << std::endl;
}

bool Cluster::isServerFd(int fd) {
	for (size_t i = 0; i < _servers.size(); i++) {
		if (_servers[i]->getFd() == fd)
			return true;
	}
	return false;
}

ssize_t Cluster::findClient(int fd) {
	for (size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i]->getFd() == fd)
			return i;
	}
	return -1;
}

ssize_t Cluster::findCgi(int fd) {
	for (size_t i = 0; i < _cgis.size(); i++) {
		if (_cgis[i]->getFd() == fd)
			return i;
	}
	return -1;
}

void Cluster::addToEpoll(int fd, __uint32_t events) {
	struct epoll_event	event;
	event.events = events;
	event.data.fd = fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
		throw std::runtime_error("epoll_ctl failed when adding fd");
}

void Cluster::deleteFromEpoll(int fd) {
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, 0) < 0)
		throw std::runtime_error("epoll_ctl failed when removing fd");
}

void Cluster::modifyEvents(int fd, __uint32_t events) {
	struct epoll_event	event;
	event.events = events;
	event.data.fd = fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &event) < 0)
		throw std::runtime_error("epoll_ctl failed when modifying fd events");
}

void Cluster::setNonBlocking(int fd) {
	int	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("Getting flags with fcntl failed");
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("Setting flags with fcntl failed");
}

void Cluster::handleClient(int fd) {
	int	client_fd = 0;

	for (size_t i = 0; i < _servers.size(); i++) {
		if (_servers[i]->getFd() == fd) {
			client_fd = _servers[i]->acceptConnection();
			if (client_fd < 0)
				break ;

			setNonBlocking(client_fd);

			if (findClient(fd) != -1)
				break ;

			addToEpoll(client_fd, EPOLLIN | EPOLLOUT | EPOLLRDHUP);

			std::map<std::string, Server*> servers;

			for (size_t j = 0; j < _servers.size(); j++) {
				if (_servers[j]->getFd() == _servers[i]->getFd()) {
					std::string hostname = _servers[j]->getIp() + ":" + _servers[j]->getPort();
					servers[hostname] = _servers[j];
				}
			}

			Client *new_client = new Client(client_fd, servers);
			_clients.push_back(new_client);

			std::cout << COL_BLUE << "==============================" << std::endl;
			std::cout << "   📡 New Client Connection   " << std::endl;
			std::cout << "==============================" << COL_RESET << std::endl << std::endl;
			std::cout << " 🔵 [INFO] Connection established with client on server " << COL_CYAN << _servers[i]->getName() << COL_RESET << std::endl;
			std::cout << std::endl;

			break ;
		}
	}
}

void Cluster::handleRequest(int fd) {
	Client	*client = _clients[findClient(fd)];
	Request	*request = client->getRequest();

	if (!request) {
		request = new Request(fd, client, *this);
		client->setRequest(request);
	}

	try {
		request->readAndParseRequest();
	} catch (const std::exception &_) {
		disconnectClient(fd);
		return ;
	}

	if (request->getIsComplete()) {
		request->handleRequest();

		std::cout << COL_GREEN << "==============================" << std::endl;
		std::cout << "      📥 Client Request       " << std::endl;
		std::cout << "==============================" << COL_RESET << std::endl << std::endl;
		std::cout << " 🟢 [REQUEST] " << request->getMethod() << " received!" << std::endl;
		std::cout << std::endl;

		client->setResponseReady(true);

		modifyEvents(fd, EPOLLOUT | EPOLLRDHUP);
	}
}

void Cluster::handleResponse(int fd) {
	std::cout << COL_CYAN << "==============================" << std::endl;
	std::cout << "      📤 Server Response      " << std::endl;
	std::cout << "==============================" << COL_RESET << std::endl << std::endl;

	Client			*client = _clients[findClient(fd)];
	Request			*request = client->getRequest();
	const Response	&response = request->getResponse();

	std::string message = response.getResponseStr();
	size_t total_size = message.size();

	ssize_t bytes_sent = send(fd, message.c_str() + client->bytes_sent, total_size - client->bytes_sent, 0);
	if (bytes_sent > 0)
		client->bytes_sent += static_cast<size_t>(bytes_sent);
	else {
		std::cerr << " 🟠 [WARNING] Failed sending data to client" << std::endl << std::endl;
		disconnectClient(fd);
		return ;
	}

	if (client->bytes_sent == total_size) {
		modifyEvents(fd, EPOLLIN | EPOLLRDHUP);
		std::cout << " 🔵 [STATUS] " << response.getCode() << std::endl;
		std::cout << " 🔵 [MESSAGE] " << response.getMessage() << std::endl;

		std::map<std::string, std::string> headers = request->getHeaders();
		if (headers.find("connection") != headers.end() && headers["connection"] == "close") {
			std::cout << std::endl;
			disconnectClient(fd);
			return ;
		}

		std::cout << std::endl;
		delete request;
		client->setRequest(NULL);
		client->setResponseReady(false);
		client->bytes_sent = 0;
	} else
		modifyEvents(fd, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
}

void Cluster::disconnectClient(int fd) {
	Client			*client = _clients[findClient(fd)];

	std::cout << COL_MAGENTA << "==============================" << std::endl;
	std::cout << "     🔌 Client Disconnect     " << std::endl;
	std::cout << "==============================" << COL_RESET << std::endl << std::endl;

	deleteFromEpoll(fd);
	_clients.erase(_clients.begin() + findClient(fd));

	delete client;

	std::cout << " 🟣 [INFO] Client has disconnected." << std::endl;
	std::cout << std::endl;
}

void Cluster::generateErrorResponse(Response &response, int code, std::string message, std::string warning) {
	if (!warning.empty())
		std::cerr << " 🟠 [WARNING] " << warning << std::endl << std::endl;
	response.setCode(code);
	response.setMessage(message);
}

void Cluster::start() {
	struct epoll_event	events[20];

	for (size_t i = 0; i < _servers.size(); i++) {
		_servers[i]->init();
		addToEpoll(_servers[i]->getFd(), EPOLLIN);
	}

	running = true;
	signal(SIGINT, handleSignal);

	displayServerInfo();

	while (running)
	{
		int events_num = epoll_wait(_epoll_fd, events, 20, -1);
		if (events_num < 0) {
			if (!running)
				break ;
			throw std::runtime_error("epoll_wait failed in cluster loop");
		}
		for (int i = 0; i < events_num; i++) {
			int fd = events[i].data.fd;
			if (isServerFd(fd) && (events[i].events & EPOLLIN))
				handleClient(fd);
			else if (findCgi(fd) != -1 && events[i].events & EPOLLIN)
				readCgiOutput(fd);
			else {
				if (events[i].events & EPOLLERR)
					disconnectClient(fd);
				else if (events[i].events & EPOLLRDHUP)
					disconnectClient(fd);
				else {
					if (events[i].events & EPOLLIN)
						handleRequest(fd);
					if (findClient(fd) != -1 && (events[i].events & EPOLLOUT) && _clients[findClient(fd)]->isResponseReady()
						&& _cgis.empty())
						handleResponse(fd);
				}
			}
		}
	}

	closeCluster(true);
}

void Cluster::closeCluster(bool print) {
	if (print) {
		std::cout << std::endl;
		std::cout << COL_BLUE << "==============================" << std::endl;
		std::cout << "    ❌ Server Shutdown ❌    " << std::endl;
		std::cout << "==============================" << COL_RESET << std::endl << std::endl;
	}

	for (size_t i = 0; i < _servers.size(); i++)
		delete _servers[i];

	for (size_t i = 0; i < _clients.size(); i++)
		delete _clients[i];

	for (size_t i = 0; i < _cgis.size(); i++)
		delete _cgis[i];

	close(_epoll_fd);

	if (print) {
		std::cout << " 🔵 [INFO] Server shut down succesfully. All connections closed." << std::endl;
		std::cout << std::endl;
	}
}

void Cluster::closeFds() {
	for (size_t i = 0; i < _servers.size(); i++)
		close(_servers[i]->getFd());

	for (size_t i = 0; i < _clients.size(); i++)
		close(_clients[i]->getFd());

	for (size_t i = 0; i < _cgis.size(); i++) {
		close(_cgis[i]->getFd());
	}

	close(_epoll_fd);
}

void Cluster::readCgiOutput(int fd) {
	Cgi			*cgi = _cgis[findCgi(fd)];
	Request		*request = cgi->getRequest();
	char		buffer[1024];
	std::string	res;
	ssize_t		bytes_read;

	do {
		bytes_read = read(fd, buffer, 1024);
		if (bytes_read < 0) {
			Response &response = request->getResponse();

			generateErrorResponse(response, 502,
				"System function call failed", "Failed to read cgi output");

			_cgis.erase(_cgis.begin() + findCgi(fd));
			delete cgi;
		} else if (bytes_read == 0)
			break ;
		res.append(buffer, bytes_read);
	} while (bytes_read > 0);

	deleteFromEpoll(fd);
	request->setResponseStr("HTTP/1.1 200 OK\r\n" + res);
	_cgis.erase(_cgis.begin() + findCgi(fd));
	delete cgi;
}

int Cluster::checkCgiTimeout(Cgi *cgi) {
	while (true) {
		int		status;
		time_t	time = std::time(NULL);
		int id = waitpid(cgi->getPid(), &status, WNOHANG);

		if (time - cgi->getStartTime() > TIMEOUT) {
			Response	&response = cgi->getClient()->getRequest()->getResponse();

			kill(cgi->getPid(), SIGKILL);

			generateErrorResponse(response, 504,
				"CGI timed out.", "CGI timed out");

			deleteFromEpoll(cgi->getFd());
			delete cgi;
			return 1;
		}

		if (id == 0) {
			continue;
		} else if (id == -1) {
			deleteFromEpoll(cgi->getFd());
			delete cgi;
			throw std::runtime_error("waitpid failed when waiting for CGI process");
		} else {
			if (WIFEXITED(status)) {
				Response	&response = cgi->getClient()->getRequest()->getResponse();

				int exit_code = WEXITSTATUS(status);
				if (exit_code == 0)
					return 0;
				else if (exit_code == 2)
					generateErrorResponse(response, 500,
						"System function call fail", "Child process system function call failed");
				else {
					generateErrorResponse(response, 502,
						"CGI execution failed.", "CGI execution failed");
					perror("Error :");
				}
			}

			deleteFromEpoll(cgi->getFd());
			delete cgi;
			return 1;
		}
	}
}

void Cluster::executeCgi(Client *client) {
	Request		*request = client->getRequest();
	int			pipe_out[2];
	int			pipe_in[2];
	std::string	cgi_path = request->getTargetFile();

	if (pipe(pipe_out) < 0) {
		generateErrorResponse(request->getResponse(), 500,
			"System function call fail", "pipe function failed when executing CGI");
		return ;
	}
	if (pipe(pipe_in) < 0) {
		close(pipe_in[0]);
		close(pipe_in[1]);
		generateErrorResponse(request->getResponse(), 500,
			"System function call fail", "pipe function failed when executing CGI");
		return ;
	}

	addToEpoll(pipe_out[0], EPOLLIN);

	int	pid = fork();
	if (pid < 0) {
		deleteFromEpoll(pipe_out[0]);

		close(pipe_out[0]);
		close(pipe_out[1]);
		close(pipe_in[0]);
		close(pipe_in[1]);

		generateErrorResponse(request->getResponse(), 500,
			"System function call fail", "fork function failed when executing CGI");
	} else if (!pid) {
		running = false;
		if (dup2(pipe_out[1], STDOUT_FILENO) < 0 || dup2(pipe_in[0], STDIN_FILENO) < 0)
			throw std::logic_error("Dup2 failed when executing CGI: " + cgi_path);

		close(pipe_out[0]);
		close(pipe_out[1]);
		close(pipe_in[0]);
		close(pipe_in[1]);

		std::string path = cgi_path.substr(0, cgi_path.find_last_of('/'));
		std::string file = "." + cgi_path.substr(cgi_path.find_last_of('/'));

		if (chdir(path.c_str()) < 0)
			throw std::logic_error("Couldnt open CGI directory");

		char *args[] = {const_cast<char *>(file.c_str()), NULL};
		char **env = generateEnv(client);

		closeFds();
		execve(file.c_str(), args, env);
		freeEnv(env);
		throw std::exception();
	} else {
		if (!request->getBody().empty()) {
			close(pipe_out[1]);
			close(pipe_in[0]);

			ssize_t bytes_written = write(pipe_in[1], request->getBody().c_str(), request->getBody().size());

			if (bytes_written <= 0) {
				close(pipe_in[1]);
				generateErrorResponse(request->getResponse(), 500, "System function call fail", "write failed when writing to cgi");
			}

			close(pipe_in[1]);

		}

		Cgi *cgi = new Cgi(pipe_out[0], pid, client, std::time(NULL));

		if (checkCgiTimeout(cgi) == 0)
			_cgis.push_back(cgi);
	}
}

char **Cluster::generateEnv(Client *client) {
	Request								*request = client->getRequest();
	std::map<std::string, std::string>	headers = request->getHeaders();
	std::istringstream					iss(request->getBody());
	std::string 						var;
	std::vector<std::string>			args;

	while (getline(iss, var, '&'))
		args.push_back(var);
	args.push_back("SERVER_PROTOCOL=HTTP/1.1");
	args.push_back("UPLOAD_SAVE=" + request->getLocation()->upload_save);
	args.push_back("SERVER_NAME=" + request->getServer()->getName());
	args.push_back("REQUEST_METHOD=" + request->getMethod());
	if (request->getHeaders().find("content-type") != request->getHeaders().end())
		args.push_back("CONTENT_TYPE=" + headers["content-type"]);
	if (request->getHeaders().find("content-length") != request->getHeaders().end())
		args.push_back("CONTENT_LENGTH=" + headers["content-length"]);

	char **env = new char*[args.size() + 1];

	size_t i;
	for (i = 0; i < args.size(); i++) {
		env[i] = new char[args[i].size() + 1];
		std::strcpy(env[i], args[i].c_str());
	}
	env[i] = NULL;

	return env;
}

void Cluster::freeEnv(char **env) {
	if (env) {
		for (size_t i = 0; env[i] != NULL; ++i)
			delete[] env[i];
		delete[] env;
	}
}




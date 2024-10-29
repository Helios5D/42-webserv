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

void	Cluster::addToEpoll(int fd, __uint32_t events) {
	struct epoll_event	event;
	event.events = events;
	event.data.fd = fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
		throw std::runtime_error("epoll_ctl failed when adding fd");
}

void	Cluster::modifyEvents(int fd, __uint32_t events) {
	struct epoll_event	event;
	event.events = events;
	event.data.fd = fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &event) < 0)
		throw std::runtime_error("epoll_ctl failed when adding fd");
}

void	Cluster::setNonBlocking(int fd) {
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

			addToEpoll(client_fd, EPOLLIN | EPOLLRDHUP);

			Client *new_client = new Client(client_fd, _servers[i]);
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
	Request	*request = new Request(fd, *(client->getServer()), *this);

	request->handleRequest();

	client->setRequest(request);

	std::cout << COL_GREEN << "==============================" << std::endl;
	std::cout << "      📥 Client Request       " << std::endl;
	std::cout << "==============================" << COL_RESET << std::endl << std::endl;
	std::cout << " 🟢 [REQUEST] " << request->getMethod() << " on server " << COL_CYAN << client->getServer()->getName() << COL_RESET << std::endl;
	std::cout << std::endl;

	client->setResponseReady(true);

	modifyEvents(fd, EPOLLOUT | EPOLLRDHUP);
}

void Cluster::handleResponse(int fd) {
	Client *client = _clients[findClient(fd)];
	Request *request = client->getRequest();
	const Response &response = request->getResponse();

	std::string message = response.getResponseStr();
	size_t total_size = message.size();

	ssize_t bytes_sent = send(fd, message.c_str() + client->bytes_sent, total_size - client->bytes_sent, 0);
	if (bytes_sent > 0)
		client->bytes_sent += static_cast<size_t>(bytes_sent);
	else
		throw std::runtime_error("Couldnt send data to client");

	if (client->bytes_sent == total_size) {
		modifyEvents(fd, EPOLLIN | EPOLLRDHUP);

		std::cout << COL_CYAN << "==============================" << std::endl;
		std::cout << "      📤 Server Response      " << std::endl;
		std::cout << "==============================" << COL_RESET << std::endl << std::endl;
		std::cout << " 🔵 [STATUS] " << response.getCode() << std::endl;
		std::cout << " 🔵 [MESSAGE] " << response.getMessage() << std::endl;

		std::map<std::string, std::string> headers = request->getHeaders();
		if (headers.find("connection") != headers.end() && headers["connection"] == "close") {
			std::cout << std::endl;
			disconnectClient(fd, 0);
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

void Cluster::disconnectClient(int fd, bool error) {
	Client			*client = _clients[findClient(fd)];

	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, 0) < 0)
		throw std::runtime_error("epoll_ctl failed when removing client");
	_clients.erase(_clients.begin() + findClient(fd));

	delete client;

	std::cout << COL_MAGENTA << "==============================" << std::endl;
	std::cout << "    🔌 Client Disconnected    " << std::endl;
	std::cout << "==============================" << COL_RESET << std::endl << std::endl;
	if (error)
		std::cout << " 🟠 [WARNING] Unexpected client disconnect." << std::endl;
	std::cout << " 🟣 [INFO] Client has disconnected." << std::endl;
	std::cout << std::endl;
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
			throw std::invalid_argument("epoll_wait failed in cluster loop");
		}
		for (int i = 0; i < events_num; i++) {
			int fd = events[i].data.fd;
			if (isServerFd(fd) && (events[i].events & EPOLLIN))
				handleClient(fd);
			else if (findCgi(fd) != -1 && events[i].events & EPOLLIN)
				readCgiOutput(fd);
			else {
				if (events[i].events & EPOLLERR)
					disconnectClient(fd, true);
				else if (events[i].events & EPOLLRDHUP)
					disconnectClient(fd, false);
				else {
					if (events[i].events & EPOLLIN)
						handleRequest(fd);
					if ((events[i].events & EPOLLOUT) && _clients[findClient(fd)]->isResponsReady()
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
		std::cout << "    ❌ Server Shutdown ❌  " << std::endl;
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

void Cluster::readCgiOutput(int fd) {
	Cgi			*cgi = _cgis[findCgi(fd)];
	Request		*request = cgi->getRequest();
	char		buffer[1024];
	std::string	res;
	ssize_t		bytes_read;

	do {
		bytes_read = read(fd, buffer, 1024);
		if (bytes_read < 0) {
			std::cerr << "Error reading CGI output: " << strerror(errno) << std::endl;
			throw std::runtime_error("read failed when reading cgi output");
		}
		res.append(buffer, bytes_read);
	} while (bytes_read > 0);
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, 0) < 0)
		throw std::runtime_error("epoll_ctl failed when removing cgi fd");

	request->setResponseStr(res);
	_cgis.erase(_cgis.begin() + findCgi(fd));
	delete cgi;
}

void Cluster::executeCgi(Request &request) {
	int			pipe_out[2];
	std::string	cgi_file = request.getTargetFile();

	std::cout << "Executing CGI" << std::endl;

	if (pipe(pipe_out) < 0)
		throw std::runtime_error("Pipe failed when executing CGI: " + cgi_file);

	// setNonBlocking(pipe_out[0]);
	addToEpoll(pipe_out[0], EPOLLIN);

	int	pid = fork();
	if (pid < 0)
		throw std::runtime_error("Fork failed when executing CGI: " + cgi_file);
	else if (!pid) {
		running = false;
		if (dup2(pipe_out[1], STDOUT_FILENO) < 0)
			throw std::runtime_error("Dup2 failed when executing CGI: " + cgi_file);

		close(pipe_out[0]);
		close(pipe_out[1]);

		char *args[] = {const_cast<char *>(cgi_file.c_str()), NULL};
		// char *args[] = {const_cast<char *>("hello"), NULL};
		char **env = generateEnv(request.getBody());

		closeCluster(false);
		execve(cgi_file.c_str(), args, env);
		// execve(std::string("hello").c_str(), args, env);
		std::cerr << COL_RED << "Execve failed" << COL_RESET << std::endl;
		freeEnv(env);
		std::exit(1);
	} else {
		Cgi *cgi = new Cgi(pipe_out[0], pid, &request, std::time(NULL));
		_cgis.push_back(cgi);

		close(pipe_out[1]);
	}
}

void Cluster::checkActiveCgi() {
	int		status;
	time_t	time = std::time(NULL);

	for (size_t i = 0; i < _cgis.size(); i++) {
		int id = waitpid(_cgis[i]->getPid(), &status, WNOHANG);

		if (time - _cgis[i]->getStartTime() > 2.0) {
			throw std::runtime_error("CGI timeout");
		}

		if (id == 0) {
			continue;
		} else if (id == -1) {
			throw std::runtime_error("waitpid failed when waiting for CGI process");
		} else {
			if (WIFEXITED(status)) {
				int exit_code = WEXITSTATUS(status);
				if (exit_code != 0) {
					throw std::runtime_error("CGI process failed");
				}
			}

			delete _cgis[i];
			_cgis.erase(_cgis.begin() + i);
			i--;
		}

	}
}

char **Cluster::generateEnv(std::string body) {
	std::istringstream iss(body);

	std::string var;
	std::vector<std::string> args;
	while (getline(iss, var, '&'))
		args.push_back(var);
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




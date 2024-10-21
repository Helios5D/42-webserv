#include "webserv.hpp"
// #include "Request.hpp"
#include "Cluster.hpp"

bool running = false;

void handleSignal(int signal) {
	(void)signal;
	running = false;
}

Cluster::Cluster(t_cluster_config config)
{
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd < 0)
		throw std::runtime_error("Epoll creation failed");
	for (size_t i = 0; i < config.servers.size(); i++) {
		Server server(config.servers[i]);
		_servers.push_back(server);
	}
}

Cluster::~Cluster()
{}

void Cluster::displayServerInfo() {
	std::cout << std::endl << COL_BLUE << "==============================" << std::endl;
	std::cout << "    🌐 Webserv Cluster 🌐    " << std::endl;
	std::cout << COL_BLUE << "==============================" << std::endl << std::endl;

	std::cout << " 🔵 [INFO] Cluster Info" << std::endl;
	for (size_t i = 0; i < _servers.size(); i++) {
		_servers[i].displayServerInfo();
	}
	std::cout << std::endl;

}

bool Cluster::isServerFd(int fd) {
	for (size_t i = 0; i < _servers.size(); i++) {
		if (_servers[i].getFd() == fd)
			return true;
	}
	return false;
}

void Cluster::handleClient(int fd) {
	for (size_t i = 0; i < _servers.size(); i++) {
		if (_servers[i].getFd() == fd) {
			int client_fd = _servers[i].acceptConnection();
			struct epoll_event event;
			event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP;
			event.data.fd = fd;
			if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0)
				throw std::runtime_error("epoll_ctl failed when adding client");
			_client_to_server[client_fd] = &_servers[i];
			break ;
		}
	}
	std::cout << COL_BLUE << "==============================" << std::endl;
	std::cout << "    📡 New Client Connection    " << std::endl;
	std::cout << COL_BLUE << "==============================" << std::endl;
	std::cout << " 🔵 [INFO] Connection established with client " << std::endl;
	std::cout << std::endl;
}

void Cluster::handleResponse(int fd) {
	(void)fd;
	// 	std::cout << COL_CYAN << "==============================" << std::endl;
	// std::cout << "   📤 Outgoing Server Response   " << std::endl;
	// std::cout << COL_CYAN << "==============================" << std::endl;
	// std::cout << " 🔵 [RESPONSE] Status: " << status_code << " (" << status_message << ")"
	//           << " sent to " << client_ip << std::endl;
	// std::cout << std::endl;

}

void Cluster::handleRequest(int fd) {
	(void)fd;
	// Request request(fd);
	// std::cout << COL_GREEN << "==============================" << std::endl;
	// std::cout << "   📥 Incoming Client Request   " << std::endl;
	// std::cout << COL_GREEN << "==============================" << std::endl;
	// std::cout << " 🔵 [REQUEST] " << method << " " << path << " from " << client_ip << std::endl;
	// std::cout << std::endl;

}

void Cluster::disconnectClient(int fd) {
	close(fd);
	_client_to_server.erase(fd);
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, 0) < 0)
		throw std::runtime_error("epoll_ctl failed when removing client");
	std::cout << COL_MAGENTA << "==============================" << std::endl;
	std::cout << "     🔌 Client Disconnected     " << std::endl;
	std::cout << COL_MAGENTA << "==============================" << std::endl;
	std::cout << " 🟣 [INFO] Client has disconnected." << std::endl;
	std::cout << std::endl;

}

void Cluster::start() {
	struct epoll_event events[20];

	for (size_t i = 0; i < _servers.size(); i++) {
		_servers[i].init();
		struct epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = _servers[i].getFd();
		if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _servers[i].getFd(), &event) < 0)
			throw std::runtime_error("epoll_ctl failed when adding servers");
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
			else {
				if (events[i].events & EPOLLRDHUP)
					disconnectClient(fd);
				if (events[i].events & EPOLLIN)
					handleRequest(fd);
				if (events[i].events & EPOLLOUT)
					handleResponse(fd);
			}
		}
	}
	std::cout << COL_BLUE << "==============================" << std::endl;
	std::cout << "       🔴 Server Shutdown       " << std::endl;
	std::cout << COL_BLUE << "==============================" << std::endl;
	std::cout << " 🔵 [INFO] Shutting down the server gracefully..." << std::endl;


	for (std::map<int, Server*>::iterator it = _client_to_server.begin(); it != _client_to_server.end(); it++)
		close(it->first);

	for (size_t i = 0; i < _servers.size(); i++)
		close(_servers[i].getFd());

	close(_epoll_fd);

	std::cout << " 🔵 [INFO] All connections closed." << std::endl;
	std::cout << std::endl;
}

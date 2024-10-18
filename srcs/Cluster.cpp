#include "webserv.hpp"
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

bool Cluster::isServerFd(int fd) {
	for (int i = 0; i < _servers.size(); i++) {
		if (_servers[i].getFd() == fd)
			return true;
	}
	return false;
}

void Cluster::handleClient(int fd) {
	for (Server server : _servers) {
		if (server.getFd() == fd) {
			int client_fd = server.acceptConnection();
			struct epoll_event event;
			event.events = EPOLLIN;
			event.data.fd = fd;
			if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0)
				throw std::runtime_error("epoll_ctl failed when adding client");
			_client_to_server[client_fd] = server;
			break ;
		}
	}
}

void Cluster::handleRequest(int fd) {
	(void)fd;
}

void Cluster::disconnectClient(int fd) {
	close(fd);
	_client_to_server.erase(fd);
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, 0) < 0)
		throw std::runtime_error("epoll_ctl failed when removing client");
}

void Cluster::start() {
	struct epoll_event events[10];

	std::cout << "Launching cluster." << std::endl;
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

	while (running)
	{
		int events_num = epoll_wait(_epoll_fd, events, 10, -1);
		if (events_num < 0)
			throw std::invalid_argument("epoll_wait failed in cluster loop");
		for (int i = 0; i < events_num; i++) {
			if (events[i].events) {
				int fd = events[i].data.fd;
				if (isServerFd(fd))
					handleClient(fd);
				else
					handleRequest(fd);
			}
		}
	}
}

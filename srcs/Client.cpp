#include "Client.hpp"

Client::Client(int fd, std::map<std::string, Server*> servers)
: _fd(fd), _servers(servers), _request(NULL), _response_ready(false), bytes_sent(0)
{}

Client::~Client()
{
	close(_fd);
	delete _request;
}

void Client::setRequest(Request *request) {
	_request = request;
}

void Client::setResponseReady(bool ready) {
	_response_ready = ready;
}

bool Client::isResponseReady() {
	return _response_ready;
}

int	Client::getFd() {
	return _fd;
}

std::map<std::string, Server*> Client::getServers() {
	return _servers;
}

Request *Client::getRequest() {
	return _request;
}
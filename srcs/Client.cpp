#include "Client.hpp"

Client::Client(int fd, Server *server)
: _fd(fd), _server(server), _request(NULL), _response_ready(false), bytes_sent(0)
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

void Client::setServer(Server *server) {
	_server = server;
}

int	Client::getFd() {
	return _fd;
}

Server *Client::getServer() {
	return _server;
}

Request *Client::getRequest() {
	return _request;
}
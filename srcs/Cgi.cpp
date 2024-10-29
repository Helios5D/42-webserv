#include "Cgi.hpp"

Cgi::Cgi(int fd, int pid, Client *client, time_t start_time)
: _fd(fd), _pid(pid), _client(client), _request(client->getRequest()), _start_time(start_time)
{}

Cgi::~Cgi() {
	close(_fd);
}

void Cgi::setRequest(Request *request) {
	_request = request;
}

int Cgi::getFd() {
	return _fd;
}

int Cgi::getPid() {
	return _pid;
}

Client *Cgi::getClient() {
	return _client;
}

Request *Cgi::getRequest() {
	return _request;
}

time_t Cgi::getStartTime() {
	return _start_time;
}

#include "Cgi.hpp"

Cgi::Cgi(int fd, int pid, Request *request, time_t start_time)
: _fd(fd), _pid(pid), _request(request), _start_time(start_time)
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

Request *Cgi::getRequest() {
	return _request;
}

time_t Cgi::getStartTime() {
	return _start_time;
}

#include "Cgi.hpp"

Cgi::Cgi(int fd_in, int fd_out, int pid, Client *client, time_t start_time)
: _fd_in(fd_in), _fd_out(fd_out), _pid(pid), _client(client), _request(client->getRequest()), _start_time(start_time)
{}

Cgi::~Cgi() {
	close(_fd_in);
	close(_fd_out);
}

void Cgi::setRequest(Request *request) {
	_request = request;
}

int Cgi::getFdIn() {
	return _fd_in;
}

int Cgi::getFdOut() {
	return _fd_out;
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

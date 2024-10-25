#pragma once

#include "webserv.hpp"
#include "Server.hpp"
#include "Cluster.hpp"
#include "Response.hpp"

class Request {
	public:
		Request(const int &fd, const Server &server, const Cluster &cluster);
		~Request();

		void										handleRequest();

		const std::string							&getMethod() const;
		const std::string							&getTargetFile() const;
		const std::map<std::string, std::string>	&getHeaders() const;
		const std::string							&getBody() const;
		const Response								&getResponse() const;

	private:
		const Server						&_server;
		const Cluster						&_cluster;
		Response							_response;
		std::string							_startLine;
		std::string							_method;
		std::string							_targetRoute;
		std::string							_targetFile;
		const t_location					*_location;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		long								_contentLength;

		bool	_isBody();
		bool	_checkStartLine();
		bool	_checkTarget();
		bool	_addHeader(const std::string &headerLine);
		void	_handleDelete();
		void	_handlePost();
};

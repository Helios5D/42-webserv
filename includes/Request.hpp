#pragma once

#include "webserv.hpp"
#include "Server.hpp"
#include "Cluster.hpp"
#include "Response.hpp"

class Cluster;
class Client;

class Request {
	public:
		Request(const int &fd, Client *client, Cluster &cluster);
		~Request();

		void										readAndParseRequest();
		void										handleRequest();

		const std::string							&getMethod() const;
		const std::string							&getTargetFile() const;
		const std::map<std::string, std::string>	&getHeaders() const;
		const std::string							&getBody() const;
		const Response								&getResponse() const;
		const bool									isComplete() const;
		Response									&getResponse();

		void										setResponseStr(const std::string &responseStr);

	private:
		const int							_fd;
		bool								_isBeginning;
		bool								_isComplete;
		Client								*_client;
		const Server						&_server;
		Cluster								&_cluster;
		Response							_response;
		std::string							_startLine;
		std::string							_method;
		std::string							_targetRoute;
		std::string							_targetFile;
		const t_location					*_location;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		size_t								_contentLength;
		size_t								_currentBodySize;
		bool								_isCgi;

		bool	_isBody();
		bool	_checkStartLine();
		bool	_checkTarget();
		bool	_addHeader(const std::string &headerLine);
		void	_handleDelete();
};

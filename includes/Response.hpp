#pragma once

#include "webserv.hpp"
#include "Server.hpp"

class Response {
	public:
		Response(const Server &server);
		~Response();

		void				createResponse();

		void				setCode(const int code);
		void				setMethod(const std::string &method);
		void				setFilePath(const std::string &filePath);
		void				setMessage(const std::string &message);
		void				setResponseStr(const std::string &responseStr);
		void				addHeader(const std::string &key, const std::string &value);

		int					getCode() const;
		const std::string	&getMessage() const;
		const std::string	&getResponseStr() const;

	private:
		const Server						&_server;
		int									_code;
		std::string							_method;
		std::string							_filePath;
		std::string							_message;
		std::string							_body;
		std::map<std::string, std::string>	_headers;
		std::string							_responseStr;

		void	_createBody();
		void	_findContentType();
};

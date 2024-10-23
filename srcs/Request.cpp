/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 10:12:04 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/23 16:00:28 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

bool Request::_isBody() {
	if (_headers.find("content-type") != _headers.end()
			&& _headers.find("content-length") != _headers.end())
		return true;

	return false;
}

Request::Request(const int &fd, const Server &server)
		: _server(server), _response(server), _contentLength(-1)
{
	std::string	headers;
	int			bytesRead;
	int			bufferSize = 30000;
	char		buffer[bufferSize];
	bool		isBeginning = true;
	size_t		bodyLength = 0;

	while ((bytesRead = read(fd, buffer, bufferSize)) > 0 && (isBeginning || bodyLength < (unsigned long)_contentLength)) {
		if (isBeginning) {
			const char *headersEnd = std::strstr(buffer, "\r\n\r\n");

			if (!headersEnd)
				headers.append(buffer, bytesRead);
			else {
				isBeginning = false;
				headers.append(buffer, headersEnd - buffer);

				std::stringstream ss(headers);

				if (!std::getline(ss, _startLine)) {
					_response.setMessage("Missing start line.");
					_response.setCode(400);
					return ;
				}
				if (_startLine[_startLine.length() - 1] == '\r')
					_startLine.erase(_startLine.length() - 1);

				std::string headerLine;
				while (std::getline(ss, headerLine)) {
					if (headerLine[headerLine.length() - 1] == '\r')
						headerLine.erase(headerLine.length() - 1);

					if (!_addHeader(headerLine)) {
						_response.setMessage("Malformed header.");
						_response.setCode(400);
						return ;
					}
				}

				if (!_checkStartLine())
					return ;

				if (_isBody()) {
					int bodyStartIndex = headersEnd - buffer + 4;

					if (_contentLength < bytesRead - bodyStartIndex)
						_body.append(buffer, bodyStartIndex, _contentLength);
					else
						_body.append(buffer, bodyStartIndex, std::string::npos);

					bodyLength = _body.length();
				}
			}
		} else if (_isBody()) {
			if ((unsigned long)_contentLength > bodyLength) {
				if ((unsigned long)_contentLength < bodyLength + bytesRead)
					_body.append(buffer, _contentLength - bodyLength);
				else
					_body.append(buffer);

				bodyLength = _body.length();
			}
		}
	}
}

Request::~Request() {}

bool Request::_checkTarget() {
	std::stringstream	ssTmp(_targetRoute);
	std::stringstream	ss(_targetRoute);
	std::string			route;
	std::string			newRoute("/");
	std::string			elem;

	if (_targetRoute[0] != '/') {
		_response.setMessage("Malformed route.");
		_response.setCode(400);
		return false;
	}

	while (1) {
		while (ssTmp.get() == '/')
			ss.get();

		if (!std::getline(ss, elem, '/'))
			break ;

		route = newRoute;
		if (newRoute == "/")
			newRoute += elem;
		else
			newRoute += '/' + elem;

		std::string tmp;
		std::getline(ssTmp, tmp, '/');
	}

	std::vector<t_location>::const_iterator it = _server.getLocations().begin();
	std::vector<t_location>::const_iterator end = _server.getLocations().end();
	for (; it != end; it++)
		if ((*it).path == newRoute)
			break ;

	if (it != end)
		_targetFile = '.' + (*it).root + '/' + (*it).index;
	else {
		for (it = _server.getLocations().begin(); it != end; it++)
			if ((*it).path == route)
				break ;

		if (it == end) {
			_response.setMessage("The resource you're looking for does not exist.");
			_response.setCode(404);
			return false;
		}

		_targetFile = '.' + (*it).root + '/' + elem;
	}

	if (access(_targetFile.c_str(), F_OK) != 0) {
		_response.setMessage("The resource you're looking for does not exist.");
		_response.setCode(404);
		return false;
	}

	_response.setFilePath(_targetFile);
	return true;
}

bool Request::_checkStartLine() {
	std::stringstream ss(_startLine);

	std::string version;
	std::string tmp;
	if (!std::getline(ss, _method, ' ') || !std::getline(ss, _targetRoute, ' ') || !std::getline(ss, version, ' ') || std::getline(ss, tmp)) {
		_response.setMessage("Malformed start line.");
		_response.setCode(400);
		return false;
	}

	if (_method != "GET" && _method != "POST" && _method != "DELETE") {
		_response.setMessage("The requested HTTP method is not allowed.");
		_response.setCode(405);
		return false;
	}

	_response.setMethod(_method);

	if (!_checkTarget())
		return false;

	if (version != "HTTP/1.1") {
		_response.setMessage("The HTTP version used is not supported by the server.");
		_response.setCode(505);
		return false;
	}

	return true;
}

bool isValidKey(const std::string &key) {
	size_t keyLen = key.length();

	if (key == "")
		return false;
	for (size_t i = 0; i < keyLen; i++)
		if (!std::isdigit(key[i]) && !std::isalpha(key[i])
				&& key[i] != '!' && key[i] != '#' && key[i] != '$'
				&& key[i] != '%' && key[i] != '&' && key[i] != '\''
				&& key[i] != '*' && key[i] != '+' && key[i] != '-'
				&& key[i] != '.' && key[i] != '^' && key[i] != '_'
				&& key[i] != '`' && key[i] != '|' && key[i] != '~')
			return false;
	return true;
}

bool isValidValue(const std::string &value) {
	size_t valueLen = value.length();

	for (size_t i = 0; i < valueLen; i++)
		if (value[i] < 9 || (value[i] > 9 && value[i] < 32) || value[i] > 126)
			return false;
	return true;
}

bool Request::_addHeader(const std::string &headerLine) {
	std::stringstream	ss(headerLine);
	std::string			key;
	std::string			value;

	if (!std::getline(ss, key, ':'))
		return false;
	std::getline(ss, value);

	if (!isValidKey(key) || _headers.find(key) != _headers.end())
		return false;

	trim(value);
	toLower(key);
	if (!isValidValue(value))
		return false;

	if (key == "content-length") {
		size_t valueLen = value.length();
		for (size_t i = 0; i < valueLen; i++)
			if (!std::isdigit(value[i]))
				return false;
		if (valueLen > 10)
			return false;
		_contentLength = std::atol(value.c_str());
		if (_contentLength > INT_MAX)
			return false;
	}

	_headers[key] = value;
	return true;
}

void Request::handleRequest() {
	_response.createResponse();
}

const std::string &Request::getMethod() const {
	return _method;
}

const std::string &Request::getTargetFile() const {
	return _targetFile;
}

const std::map<std::string, std::string> &Request::getHeaders() const {
	return _headers;
}

const std::string &Request::getBody() const {
	return _body;
}

const Response &Request::getResponse() const {
	return _response;
}

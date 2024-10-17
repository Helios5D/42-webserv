/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 10:12:04 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/17 17:35:58 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(const std::string &request) : _contentLength(-1) {
	std::stringstream ss(request);

	if (!std::getline(ss, _startLine)) {
		_response = "Missing start line.";
		_resCode = 400;
		return ;
	}
	if (!_checkStartLine())
		return ;

	std::string headerLine;
	while (std::getline(ss, headerLine) && headerLine != "") {
		if (!_addHeader(headerLine)) {
			_response = "Malformed header.";
			_resCode = 400;
			return ;
		}
	}

	std::string	bodyLine;
	long		bodyLen = 0;
	while (std::getline(ss, bodyLine)) {
		if (_headers.find("content-type") == _headers.end()) {
			_response = "Body present but Content-Type header is missing.";
			_resCode = 400;
			return ;
		}

		size_t line_len = bodyLine.length();
		for (size_t i = 0; i < line_len; i++) {
			if (bodyLen == _contentLength)
				break ;
			_body += bodyLine[i];
			bodyLen++;
		}
		if (bodyLen != _contentLength) {
			_body += '\n';
			bodyLen++;
		}
	}
	_checkBody();
}

Request::~Request() {}

bool Request::_checkStartLine() {
	std::stringstream ss(_startLine);

	std::string version;
	std::string tmp;
	if (!std::getline(ss, _method, ' ') || !std::getline(ss, _target, ' ') || !std::getline(ss, version, ' ') || std::getline(ss, tmp)) {
		_resCode = 400;
		_response = "Malformed start line.";
		return false;
	}

	if (_method != "GET" && _method != "POST" && _method != "DELETE") {
		_resCode = 405;
		_response = "The requested HTTP method is not allowed.";
		return false;
	}

	if (version != "HTTP/1.1") {
		_resCode = 505;
		_response = "The HTTP version used is not supported by the server.";
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
	toLower(value);
	if (!isValidValue(value))
		return false;

	if (key == "content-length") {
		size_t valueLen = value.length();
		for (size_t i = 0; i < valueLen; i++)
			if (!std::isdigit(value[i]))
				return false;
		if (valueLen > 10)
			return false;
		_contentLength = atol(value.c_str());
		if (_contentLength > INT_MAX)
			return false;
	}

	_headers[key] = value;
	return true;
}

bool Request::_checkBody() {
	return true;
}

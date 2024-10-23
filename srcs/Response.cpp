/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 10:44:01 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/23 14:07:55 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(const Server &server)
		: _server(server), _code(200), _file("./pages/fallback.html") {}

Response::~Response() {}

void Response::createResponse() {
	std::string								codeMessage = "Error";
	std::map<int, std::string>::iterator	resIt = g_codes_responses.find(_code);
	std::stringstream						ss;

	if (resIt != g_codes_responses.end())
		codeMessage = (*resIt).second;

	ss << "HTTP/1.1 " << _code << ' ' << codeMessage << "\r\n";

	std::map<std::string, std::string>::iterator it = _headers.begin();
	std::map<std::string, std::string>::iterator end = _headers.end();

	for (; it != end; it++)
		ss << (*it).first << ":" << (*it).second << "\r\n";

	ss << "\r\n" << _body;

	_response = ss.str();
}

void Response::_replaceBodyMessage() {
	std::string from = "<p>MESSAGE</p>";
	std::string to = "<p>" + _message + "</p>";
	std::string::size_type pos = 0;

	while ((pos = _body.find(from, pos)) != std::string::npos) {
		_body.replace(pos, from.length(), to);
		pos += to.length();
	}
}

void Response::_setBody() {
	if (_code == 200) {

	} else {
		std::map<int, std::string> errorPages = _server.getErrorPages();
		std::map<int, std::string>::const_iterator it = errorPages.find(_code);

		if (it != errorPages.end()) {
			std::ifstream file((*it).second.c_str());

			if (file.is_open()) {
				std::stringstream buf;

				buf << file.rdbuf();
				_body = buf.str();
				_replaceBodyMessage();

				_headers["content-type"] = "text/html";
				file.close();
			} else {
				_body = _message;
				_headers["content-type"] = "text/plain";
			}
		} else {
			_body = _message;
			_headers["content-type"] = "text/plain";
		}

		nbToStr(_headers["content-length"], _body.length());
	}
}

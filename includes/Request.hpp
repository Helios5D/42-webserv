/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdaher <hdaher@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 10:10:56 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/22 15:51:30 by hdaher           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Server.hpp"

class Request {
	public:
		Request(const int &fd, const Server &server);
		Request(const Request &other);

		Request &operator=(const Request &rhs);

		~Request();

		const std::string							&getMethod() const;
		const std::string							&getTargetFile() const;
		const std::map<std::string, std::string>	&getHeaders() const;
		const std::string							&getBody() const;
		const std::string							&getResponse() const;
		int											getResCode() const;

	private:
		const Server						&_server;
		std::string							_startLine;
		std::string							_method;
		std::string							_targetRoute;
		std::string							_targetFile;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		std::string							_response;
		int									_resCode;
		long								_contentLength;

		bool	_isBody();
		bool	_checkStartLine();
		bool	_checkTarget();
		bool	_addHeader(const std::string &headerLine);
};

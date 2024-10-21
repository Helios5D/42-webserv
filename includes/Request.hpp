/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 10:10:56 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/21 16:19:12 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Server.hpp"

class Request {
	public:
		Request(const int &fd, const Server &server);
		~Request();
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

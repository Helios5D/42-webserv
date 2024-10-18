/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 10:10:56 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/18 16:27:15 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class Request {
	public:
		Request(const int &fd);
		~Request();
	private:
		std::string							_startLine;
		std::string							_method;
		std::string							_target;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		std::string							_response;
		int									_resCode;
		long								_contentLength;

		bool	_isBody();
		bool	_checkStartLine();
		bool	_addHeader(const std::string &headerLine);
		bool	_checkBody();
};

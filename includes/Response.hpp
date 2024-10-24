/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 10:29:53 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/23 16:32:12 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Server.hpp"

class Response {
	public:
		Response(const Server &server);
		~Response();

		void				createResponse();

		void				setCode(int code);
		void				setMethod(std::string method);
		void				setFilePath(std::string filePath);
		void				setMessage(std::string message);

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
		void	_replaceBodyMessage();
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 10:29:53 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/23 11:53:27 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Server.hpp"

class Response {
	public:
		Response(const Server &server);
		~Response();

		void	createResponse();

	private:
		const Server						&_server;
		int									_code;
		std::string							_file;
		std::string							_message;
		std::string							_body;
		std::map<std::string, std::string>	_headers;
		std::string							_response;

		void	_setBody();
		void	_replaceBodyMessage();
};

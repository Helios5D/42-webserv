/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdaher <hdaher@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 15:32:20 by hdaher            #+#    #+#             */
/*   Updated: 2024/10/18 16:00:54 by hdaher           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

std::string readRequest(int request_fd) {
	std::string request;
	char buffer[1024];
	int ret;
	while ((ret = read(request_fd, buffer, 1024)) > 0)
		request.append(buffer, ret);
	if (ret < 0)
		throw std::runtime_error("Reading request failed");
	return (request);
}

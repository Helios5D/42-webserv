/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 15:01:02 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/17 15:18:29 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void trim(std::string &str) {
	size_t start = str.find_first_not_of(" \t");
	if (start == std::string::npos)
		str = "";
	else {
		size_t end = str.find_last_not_of(" \t");
		str = str.substr(start, end - start + 1);
	}
}

void toLower(std::string &str) {
	size_t strLen = str.length();
	for (size_t i = 0; i < strLen; i++)
		if (std::isalpha(str[i]))
			str[i] = std::tolower(str[i]);
}

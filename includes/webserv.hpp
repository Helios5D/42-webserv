/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 11:52:47 by hdaher            #+#    #+#             */
/*   Updated: 2024/10/23 13:43:45 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Includes //

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>

#include <climits>
#include <cstring>
#include <csignal>
#include <cstdlib>

#include "colors.hpp"
#include "config.hpp"

// Prototypes //

// tools.cpp
void	trim(std::string &line);
void	toLower(std::string &str);
void	nbToStr(std::string &str, size_t nb);
void	initializeCodesResponses();

// Globals //

extern std::map<int, std::string> g_codes_responses;

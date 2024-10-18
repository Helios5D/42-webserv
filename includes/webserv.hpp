/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdaher <hdaher@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 11:52:47 by hdaher            #+#    #+#             */
/*   Updated: 2024/10/18 15:53:17 by hdaher           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Includes //

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <csignal>
#include <poll.h>
#include <vector>
#include <map>
#include <cstdlib>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <climits>
#include <cstring>
#include "colors.hpp"

// Prototypes //

std::string	readRequest(int request_fd);

// tools.cpp
void		trim(std::string &line);
void		toLower(std::string &str);

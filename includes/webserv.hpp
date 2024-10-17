/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 11:52:47 by hdaher            #+#    #+#             */
/*   Updated: 2024/10/17 17:28:56 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Includes //

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <poll.h>
#include <vector>
#include <map>
#include <cstdlib>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <climits>
#include "colors.hpp"

// Prototypes //

std::string	readRequest(int request_fd);

// tools.cpp
void		trim(std::string &line);
void		toLower(std::string &str);

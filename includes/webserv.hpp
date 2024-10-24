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
#include <algorithm>

#include <climits>
#include <cstring>
#include <csignal>
#include <cstdlib>

#include "colors.hpp"
#include "config.hpp"

// Prototypes //

// tools.cpp
bool	isNumber(const std::string str);
void	trim(std::string &line);
void	toLower(std::string &str);
void	nbToStr(std::string &str, size_t nb);
void	initializeCodesResponses();
void	initializeContentTypes();

// Globals //

extern std::map<int, std::string> g_codes_responses;
extern std::map<std::string, std::string> g_content_types;

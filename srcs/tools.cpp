/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrochedy <mrochedy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 15:01:02 by mrochedy          #+#    #+#             */
/*   Updated: 2024/10/23 13:43:24 by mrochedy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

std::map<int, std::string> g_codes_responses;

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

void nbToStr(std::string &str, size_t nb) {
	std::stringstream ss;
	ss << nb;
	str = ss.str();
}

void initializeCodesResponses() {
	g_codes_responses[100] = "Continue";
	g_codes_responses[101] = "Switching Protocols";
	g_codes_responses[102] = "Processing";
	g_codes_responses[103] = "Early Hints";
	g_codes_responses[200] = "OK";
	g_codes_responses[201] = "Created";
	g_codes_responses[202] = "Accepted";
	g_codes_responses[203] = "Non-Authoritative Information";
	g_codes_responses[204] = "No Content";
	g_codes_responses[205] = "Reset Content";
	g_codes_responses[206] = "Partial Content";
	g_codes_responses[207] = "Multi-Status";
	g_codes_responses[208] = "Already Reported";
	g_codes_responses[210] = "Content Different";
	g_codes_responses[226] = "IM Used";
	g_codes_responses[300] = "Multiple Choices";
	g_codes_responses[301] = "Moved Permanently";
	g_codes_responses[302] = "Found";
	g_codes_responses[303] = "See Other";
	g_codes_responses[304] = "Not Modified";
	g_codes_responses[305] = "Use Proxy";
	g_codes_responses[307] = "Temporary Redirect";
	g_codes_responses[308] = "Permanent Redirect";
	g_codes_responses[310] = "Too many Redirects";
	g_codes_responses[400] = "Bad Request";
	g_codes_responses[401] = "Unauthorized";
	g_codes_responses[402] = "Payment Required";
	g_codes_responses[403] = "Forbidden";
	g_codes_responses[404] = "Not Found";
	g_codes_responses[405] = "Method Not Allowed";
	g_codes_responses[406] = "Not Acceptable";
	g_codes_responses[407] = "Proxy Authentication Required";
	g_codes_responses[408] = "Request Time-out";
	g_codes_responses[409] = "Conflict";
	g_codes_responses[410] = "Gone";
	g_codes_responses[411] = "Length Required";
	g_codes_responses[412] = "Precondition Failed";
	g_codes_responses[413] = "Request Entity Too Large";
	g_codes_responses[414] = "Request-URI Too Long";
	g_codes_responses[415] = "Unsupported Media Type";
	g_codes_responses[416] = "Requested range unsatisfiable";
	g_codes_responses[417] = "Expectation failed";
	g_codes_responses[418] = "I'm a teapot";
	g_codes_responses[419] = "Page expired";
	g_codes_responses[421] = "Bad mapping / Misdirected Request";
	g_codes_responses[422] = "Unprocessable entity";
	g_codes_responses[423] = "Locked";
	g_codes_responses[424] = "Method failure";
	g_codes_responses[425] = "Too Early";
	g_codes_responses[426] = "Upgrade Required";
	g_codes_responses[427] = "Invalid digital signature";
	g_codes_responses[428] = "Precondition Required";
	g_codes_responses[429] = "Too Many Requests";
	g_codes_responses[431] = "Request Header Fields Too Large";
	g_codes_responses[449] = "Retry With";
	g_codes_responses[450] = "Blocked by Windows Parental Controls";
	g_codes_responses[451] = "Unavailable For Legal Reasons";
	g_codes_responses[456] = "Unrecoverable Error";
	g_codes_responses[444] = "No Response";
	g_codes_responses[495] = "SSL Certificate Error";
	g_codes_responses[496] = "SSL Certificate Required";
	g_codes_responses[497] = "HTTP Request Sent to HTTPS Port";
	g_codes_responses[498] = "Token expired/invalid";
	g_codes_responses[499] = "Client Closed Request";
	g_codes_responses[500] = "Internal Server Error";
	g_codes_responses[501] = "Not Implemented";
	g_codes_responses[502] = "Bad Gateway or Proxy Error";
	g_codes_responses[503] = "Service Unavailable";
	g_codes_responses[504] = "Gateway Time-out";
	g_codes_responses[505] = "HTTP Version not supported";
	g_codes_responses[506] = "Variant Also Negotiates";
	g_codes_responses[507] = "Insufficient storage";
	g_codes_responses[508] = "Loop detected";
	g_codes_responses[509] = "Bandwidth Limit Exceeded";
	g_codes_responses[510] = "Not extended";
	g_codes_responses[511] = "Network authentication required";
	g_codes_responses[520] = "Unknown Error";
	g_codes_responses[521] = "Web Server Is Down";
	g_codes_responses[522] = "Connection Timed Out";
	g_codes_responses[523] = "Origin Is Unreachable";
	g_codes_responses[524] = "A Timeout Occurred";
	g_codes_responses[525] = "SSL Handshake Failed";
	g_codes_responses[526] = "Invalid SSL Certificate";
	g_codes_responses[527] = "Railgun Error";
}

#include "webserv.hpp"

std::map<int, std::string>			g_codes_responses;
std::map<std::string, std::string>	g_content_types;

bool isNumber(const std::string str) {
	size_t size = str.size();
	for (size_t i = 0; i < size; i++)
		if (!std::isdigit(str[i]))
			return false;
	return true;
}

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

void getFileExtension(const std::string &filePath, std::string &extension) {
	size_t lastPointIndex = filePath.find_last_of('.');

	if (lastPointIndex != std::string::npos)
		extension = filePath.substr(lastPointIndex + 1);
}

void generateFileName(std::string &fileName) {
	std::stringstream	ss;
	std::time_t			t = std::time(0);
	std::tm*			now = std::localtime(&t);

	ss << now->tm_year + 1900 << '-' << now->tm_mon + 1 << '-' << now->tm_mday << '_'
		<< now->tm_hour << '-' << now->tm_min << '-' << now->tm_sec;

	fileName = ss.str();
}

bool isDirectory(const std::string &path) {
	struct stat pathStat;

	if (stat(path.c_str(), &pathStat) != 0)
		return 0;
	return S_ISDIR(pathStat.st_mode);
}

void replaceFirstOccurence(std::string &str, const std::string &from, const std::string &to) {
	std::string::size_type pos = 0;

	if ((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}
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

void initializeContentTypes() {
	g_content_types["aac"] = "audio/aac";
	g_content_types["abw"] = "application/x-abiword";
	g_content_types["apng"] = "image/apng";
	g_content_types["arc"] = "application/x-freearc";
	g_content_types["avif"] = "image/avif";
	g_content_types["avi"] = "video/x-msvideo";
	g_content_types["azw"] = "application/vnd.amazon.ebook";
	g_content_types["bin"] = "application/octet-stream";
	g_content_types["bmp"] = "image/bmp";
	g_content_types["bz"] = "application/x-bzip";
	g_content_types["bz2"] = "application/x-bzip2";
	g_content_types["cda"] = "application/x-cdf";
	g_content_types["csh"] = "application/x-csh";
	g_content_types["css"] = "text/css";
	g_content_types["csv"] = "text/csv";
	g_content_types["doc"] = "application/msword";
	g_content_types["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	g_content_types["eot"] = "application/vnd.ms-fontobject";
	g_content_types["epub"] = "application/epub+zip";
	g_content_types["gz"] = "application/gzip";
	g_content_types["gif"] = "image/gif";
	g_content_types["htm"] = "text/html";
	g_content_types["html"] = "text/html";
	g_content_types["ico"] = "image/vnd.microsoft.icon";
	g_content_types["ics"] = "text/calendar";
	g_content_types["jar"] = "application/java-archive";
	g_content_types["jpeg"] = "image/jpeg";
	g_content_types["jpg"] = "image/jpeg";
	g_content_types["js"] = "text/javascript";
	g_content_types["json"] = "application/json";
	g_content_types["jsonld"] = "application/ld+json";
	g_content_types["mid"] = "audio/midi";
	g_content_types["midi"] = "audio/midi";
	g_content_types["mjs"] = "text/javascript";
	g_content_types["mp3"] = "audio/mpeg";
	g_content_types["mp4"] = "video/mp4";
	g_content_types["mpeg"] = "video/mpeg";
	g_content_types["mpkg"] = "application/vnd.apple.installer+xml";
	g_content_types["odp"] = "application/vnd.oasis.opendocument.presentation";
	g_content_types["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	g_content_types["odt"] = "application/vnd.oasis.opendocument.text";
	g_content_types["oga"] = "audio/ogg";
	g_content_types["ogv"] = "video/ogg";
	g_content_types["ogx"] = "application/ogg";
	g_content_types["opus"] = "audio/ogg";
	g_content_types["otf"] = "font/otf";
	g_content_types["png"] = "image/png";
	g_content_types["pdf"] = "application/pdf";
	g_content_types["php"] = "application/x-httpd-php";
	g_content_types["ppt"] = "application/vnd.ms-powerpoint";
	g_content_types["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	g_content_types["rar"] = "application/vnd.rar";
	g_content_types["rtf"] = "application/rtf";
	g_content_types["sh"] = "application/x-sh";
	g_content_types["svg"] = "image/svg+xml";
	g_content_types["tar"] = "application/x-tar";
	g_content_types["tif"] = "image/tiff";
	g_content_types["tiff"] = "image/tiff";
	g_content_types["ts"] = "video/mp2t";
	g_content_types["ttf"] = "font/ttf";
	g_content_types["txt"] = "text/plain";
	g_content_types["vsd"] = "application/vnd.visio";
	g_content_types["wav"] = "audio/wav";
	g_content_types["weba"] = "audio/webm";
	g_content_types["webm"] = "video/webm";
	g_content_types["webp"] = "image/webp";
	g_content_types["woff"] = "font/woff";
	g_content_types["woff2"] = "font/woff2";
	g_content_types["xhtml"] = "application/xhtml+xml";
	g_content_types["xls"] = "application/vnd.ms-excel";
	g_content_types["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	g_content_types["xml"] = "application/xml";
	g_content_types["xul"] = "application/vnd.mozilla.xul+xml";
	g_content_types["zip"] = "application/zip";
	g_content_types["3gp"] = "video/3gpp";
	g_content_types["3g2"] = "video/3gpp2";
	g_content_types["7z"] = "application/x-7z-compressed";
}


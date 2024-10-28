#include "Response.hpp"

Response::Response(const Server &server)
		: _server(server), _code(200), _method("GET"), _filePath("./pages/fallback.html") {}

Response::~Response() {}

void Response::createResponse() {
	std::string								codeMessage = "Error";
	std::map<int, std::string>::iterator	resIt = g_codes_responses.find(_code);
	std::stringstream						ss;

	if (resIt != g_codes_responses.end())
		codeMessage = (*resIt).second;

	ss << "HTTP/1.1 " << _code << ' ' << codeMessage << "\r\n";

	_createBody();

	std::map<std::string, std::string>::iterator it = _headers.begin();
	std::map<std::string, std::string>::iterator end = _headers.end();

	for (; it != end; it++)
		ss << (*it).first << ":" << (*it).second << "\r\n";

	ss << "\r\n" << _body;

	_responseStr = ss.str();
}

void Response::_replaceBodyPart(const std::string &from, const std::string &to) {
	std::string::size_type pos = 0;

	while ((pos = _body.find(from, pos)) != std::string::npos) {
		_body.replace(pos, from.length(), to);
		pos += to.length();
	}
}

void Response::_findContentType() {
	std::string extension;
	getFileExtension(_filePath, extension);

	if (!extension.empty()) {
		std::map<std::string, std::string>::iterator it;
		it = g_content_types.find(extension);

		if (it != g_content_types.end())
			_headers["content-type"] = (*it).second;
		else
			_headers["content-type"] = "text/plain";
	} else
		_headers["content-type"] = "text/plain";
}

std::string listDirectory(const std::string &path) {
	std::string	directoryListing;
	DIR			*dir;

	if ((dir = opendir(path.c_str())) != NULL) {
		for (struct dirent *entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
			std::string entryName = entry->d_name;

			if (entryName == "." || entryName == "..")
				continue ;
			directoryListing += "<li>";
			entry->d_type == DT_DIR ? directoryListing += "D - " : directoryListing += "F - ";
			directoryListing += "<a href=\"" + path + '/' + entryName + "\">" + entryName + "</a></li>\n";
		}

		closedir(dir);
	} else
		throw std::runtime_error("directory could not be opened");

	return directoryListing;
}

void Response::_createBody() {
	if (_code == 200) {
		if (_method == "POST") {
			_message = "POST request was successfully processed.";
			_body = _message;
			_headers["content-type"] = "text/plain";
		} else if (_method == "DELETE") {
			_message = "File was successfully deleted.";
			_body = _message;
			_headers["content-type"] = "text/plain";
		} else {
			if (isDirectory(_filePath)) {
				std::ifstream file("./web/pages/directory_listing.html");

				if (file.is_open()) {
					std::stringstream buf;

					buf << file.rdbuf();
					_body = buf.str();
					_replaceBodyPart("[DIRECTORY LISTING]", listDirectory(_filePath));

					_headers["content-type"] = "text/html";
					file.close();
				} else {
					_body = _message;
					_headers["content-type"] = "text/plain";
				}
			} else {
				std::ifstream file(_filePath.c_str());

				if (file.is_open()) {
					std::stringstream buf;

					buf << file.rdbuf();
					_body = buf.str();

					_findContentType();
					_message = "Requested file was successfully returned.";
					file.close();
				} else {
					_code = 404;
					_message = "The resource you're looking for does not exist.";
					_createBody();
				}
			}
		}
	} else {
		std::map<int, std::string> errorPages = _server.getErrorPages();
		std::map<int, std::string>::const_iterator it = errorPages.find(_code);

		if (it != errorPages.end()) {
			std::ifstream errorPageFile((*it).second.c_str());

			if (errorPageFile.is_open()) {
				std::stringstream buf;

				buf << errorPageFile.rdbuf();
				_body = buf.str();
				_replaceBodyPart("[MESSAGE]", _message);

				_headers["content-type"] = "text/html";
				errorPageFile.close();
			} else {
				_body = _message;
				_headers["content-type"] = "text/plain";
			}
		} else {
			_body = _message;
			_headers["content-type"] = "text/plain";
		}
	}

	nbToStr(_headers["content-length"], _body.length());
}

void Response::setCode(const int code) {
	_code = code;
}

void Response::setMethod(const std::string &method) {
	_method = method;
}

void Response::setFilePath(const std::string &filePath) {
	_filePath = filePath;
}

void Response::setMessage(const std::string &message) {
	_message = message;
}

void Response::setResponseStr(const std::string &responseStr) {
	_responseStr = responseStr;
}

void Response::addHeader(const std::string &key, const std::string &value) {
	_headers[key] = value;
}

int Response::getCode() const {
	return _code;
}

const std::string &Response::getMessage() const {
	return _message;
}

const std::string &Response::getResponseStr() const {
	return _responseStr;
}

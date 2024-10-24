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

void Response::_replaceBodyMessage() {
	std::string from = "<p>MESSAGE</p>";
	std::string to = "<p>" + _message + "</p>";
	std::string::size_type pos = 0;

	while ((pos = _body.find(from, pos)) != std::string::npos) {
		_body.replace(pos, from.length(), to);
		pos += to.length();
	}
}

void Response::_findContentType() {
	std::string extension;
	size_t lastPointIndex = _filePath.find_last_of('.');

	if (lastPointIndex != std::string::npos) {
		std::map<std::string, std::string>::iterator it;

		extension = _filePath.substr(lastPointIndex + 1);
		it = g_content_types.find(extension);

		if (it != g_content_types.end())
			_headers["content-type"] = (*it).second;
		else
			_headers["content-type"] = "text/plain";
	} else
		_headers["content-type"] = "text/plain";
}

void Response::_createBody() {
	if (_code == 200) {
		if (_method == "POST") {
			_body = "POST request was successfully processed.";
			_headers["content-type"] = "text/plain";
		} else if (_method == "DELETE") {
			_body = "File was successfully deleted.";
			_headers["content-type"] = "text/plain";
		} else {
			std::ifstream file(_filePath.c_str());

			if (file.is_open()) {
				std::stringstream buf;

				buf << file.rdbuf();
				_body = buf.str();

				_findContentType();
				file.close();
			} else {
				_code = 404;
				_message = "The resource you're looking for does not exist.";
				_createBody();
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
				_replaceBodyMessage();

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

void Response::setCode(int code) {
	_code = code;
}

void Response::setMethod(std::string method) {
	_method = method;
}

void Response::setFilePath(std::string filePath) {
	_filePath = filePath;
}

void Response::setMessage(std::string message) {
	_message = message;
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

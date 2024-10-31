#include "Response.hpp"

Response::Response(const std::map<std::string, std::string> &requestHeaders)
		: _server(NULL), _code(200), _method("GET"), _filePath("./pages/fallback.html"), _requestHeaders(requestHeaders), _isCgi(false)
{}

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
			bool isDir = entry->d_type == DT_DIR;
			directoryListing += "<li>";
			isDir ? directoryListing += "D - " : directoryListing += "F - ";
			directoryListing += "<a href=\"" + entryName + (isDir ? "/" : "") + "\">" + entryName + "</a></li>\n";
		}

		closedir(dir);
	} else
		throw std::runtime_error("directory could not be opened");

	return directoryListing;
}

std::string readFile(const std::string &path) {
	std::ifstream file(path.c_str());

	if (file.is_open()) {
		std::stringstream buf;

		buf << file.rdbuf();
		file.close();
		return buf.str();
	} else
		throw std::exception();
}

void Response::_createBody() {
	if (_code == 200) {
		if (_method == "DELETE") {
			_message = "File was successfully deleted.";
			_body = _message;
			_headers["content-type"] = "text/plain";
		} else if (_isCgi) {
			_message = "Requested CGI was successfully executed.";
			_body = _message;
			_headers["content-type"] = "text/plain";
		} else {
			if (_filePath == "./web/login/login.html") {
				std::map<std::string, std::string>::const_iterator	it;
				std::string											keyEqual;
				size_t												start;

				it = _requestHeaders.find("cookie");
				keyEqual = "username=";
				if (it != _requestHeaders.end() && (start = (*it).second.find_first_of(keyEqual)) != std::string::npos) {
					start += keyEqual.length();
					std::string	username = (*it).second.substr(start, (*it).second.find(';', start) - start);

					try {
						_body = readFile("./web/login/logged_in.html");
						replaceFirstOccurence(_body, "[USER]", username);
						_headers["content-type"] = "text/html";
					} catch (const std::exception &_) {
						_code = 404;
						_message = "The resource you're looking for does not exist.";
						_createBody();
					}
				} else {
					try {
						_body = readFile("./web/login/login.html");
						_headers["content-type"] = "text/html";
					} catch (const std::exception &_) {
						_code = 404;
						_message = "The resource you're looking for does not exist.";
						_createBody();
					}
				}
			} else if (isDirectory(_filePath)) {
				try {
					_body = readFile("./web/pages/directory_listing.html");
					replaceFirstOccurence(_body, "[DIRECTORY LISTING]", listDirectory(_filePath));
					_headers["content-type"] = "text/html";
				} catch (const std::exception &_) {
					_body = _message;
					_headers["content-type"] = "text/plain";
				}
			} else {
				try {
					_body = readFile(_filePath.c_str());
					_findContentType();
					_message = "Requested file was successfully returned.";
				} catch (const std::exception &_) {
					_code = 404;
					_message = "The resource you're looking for does not exist.";
					_createBody();
				}
			}
		}
	} else {
		if (!_server)
			return ;
		std::map<int, std::string> errorPages = _server->getErrorPages();
		std::map<int, std::string>::const_iterator it = errorPages.find(_code);

		if (it != errorPages.end()) {
			try {
				_body = readFile((*it).second.c_str());
				replaceFirstOccurence(_body, "[MESSAGE]", _message);
				_headers["content-type"] = "text/html";
			} catch (const std::exception &_) {
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

void Response::setServer(Server *server) {
	_server = server;
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

void Response::setIsCgi(const bool &isCgi) {
	_isCgi = isCgi;
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

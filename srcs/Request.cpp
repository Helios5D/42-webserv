#include "Request.hpp"

bool Request::_isBody() {
	if (_headers.find("content-type") != _headers.end()
			&& _headers.find("content-length") != _headers.end())
		return true;

	return false;
}

Request::Request(const int &fd, const Server &server, Cluster &cluster)
		: _server(server), _cluster(cluster), _response(server), _location(NULL), _contentLength(-1)
{
	std::string	headers;
	int			bytesRead;
	int			bufferSize = 30000;
	char		buffer[bufferSize];
	bool		isBeginning = true;
	size_t		bodyLength = 0;

	while ((bytesRead = read(fd, buffer, bufferSize)) > 0 && (isBeginning || bodyLength < (unsigned long)_contentLength)) {
		if (isBeginning) {
			const char *headersEnd = std::strstr(buffer, "\r\n\r\n");

			if (!headersEnd)
				headers.append(buffer, bytesRead);
			else {
				isBeginning = false;
				headers.append(buffer, headersEnd - buffer);

				std::stringstream ss(headers);

				if (!std::getline(ss, _startLine)) {
					_response.setMessage("Missing start line.");
					_response.setCode(400);
					return ;
				}
				if (_startLine[_startLine.length() - 1] == '\r')
					_startLine.erase(_startLine.length() - 1);

				std::string headerLine;
				while (std::getline(ss, headerLine)) {
					if (headerLine[headerLine.length() - 1] == '\r')
						headerLine.erase(headerLine.length() - 1);

					if (!_addHeader(headerLine))
						return ;
				}

				if (!_checkStartLine())
					return ;

				if (_isBody()) {
					int bodyStartIndex = headersEnd - buffer + 4;

					if (_contentLength < bytesRead - bodyStartIndex)
						_body.append(buffer, bodyStartIndex, _contentLength);
					else
						_body.append(buffer, bodyStartIndex, std::string::npos);

					bodyLength = _body.length();
				}
			}
		} else if (_isBody()) {
			if ((unsigned long)_contentLength > bodyLength) {
				if ((unsigned long)_contentLength < bodyLength + bytesRead)
					_body.append(buffer, _contentLength - bodyLength);
				else
					_body.append(buffer);

				bodyLength = _body.length();
			}
		}
	}
}

Request::~Request() {}

bool Request::_checkTarget() {
	std::string	route = _targetRoute;
	bool		isSlash = false;

	if (_targetRoute[0] != '/') {
		_response.setMessage("Malformed route.");
		_response.setCode(400);
		return false;
	}

	std::string::iterator routeIt = route.begin();
	while (routeIt != route.end()) {
		if (*routeIt == '/') {
			if (!isSlash) {
				isSlash = true;
				routeIt++;
			} else
				routeIt = route.erase(routeIt);
		} else {
			isSlash = false;
			routeIt++;
		}
	}

	while (!route.empty()) {
		std::vector<t_location>::const_iterator it = _server.getLocations().begin();
		std::vector<t_location>::const_iterator end = _server.getLocations().end();

		for (; it != end; it++)
			if ((*it).path == route)
				break ;
		if (it != end) {
			_location = &(*it);
			if (_location->redir_code != -1) {
				_response.setCode(_location->redir_code);
				if (!_location->redir_path.empty())
					_response.addHeader("location", _location->redir_path);
				return false;
			}

			_targetFile = '.' + _targetRoute;
			replaceFirstOccurence(_targetFile, route, (*it).root + '/');

			if (_targetRoute[_targetRoute.length() - 1] == '/') {
				if (_location->autoindex == "on")
					_targetFile += '/' + (*it).index;
			} else if (isDirectory(_targetFile))
					_targetFile += '/' + (*it).index;

			if (access(_targetFile.c_str(), F_OK) != 0) {
				_response.setMessage("The resource you're looking for does not exist.");
				_response.setCode(404);
				return false;
			}

			_response.setFilePath(_targetFile);
			break ;
		}
		if (route == "/" && it == end)
			break ;

		route = route.substr(0, route.find_last_of('/'));
		if (route.empty())
			route = '/';
	}

	if (!_location) {
		_response.setMessage("The resource you're looking for does not exist.");
		_response.setCode(404);
		return false;
	}

	return true;
}

bool Request::_checkStartLine() {
	std::stringstream ss(_startLine);

	std::string version;
	std::string tmp;
	if (!std::getline(ss, _method, ' ') || !std::getline(ss, _targetRoute, ' ') || !std::getline(ss, version, ' ') || std::getline(ss, tmp)) {
		_response.setMessage("Malformed start line.");
		_response.setCode(400);
		return false;
	}

	if (_method != "GET" && _method != "POST" && _method != "DELETE") {
		_response.setMessage("The requested HTTP method is not allowed.");
		_response.setCode(405);
		return false;
	}

	_response.setMethod(_method);

	if (!_checkTarget())
		return false;

	if (version != "HTTP/1.1") {
		_response.setMessage("The HTTP version used is not supported by the server.");
		_response.setCode(505);
		return false;
	}

	return true;
}

bool isValidKey(const std::string &key) {
	size_t keyLen = key.length();

	if (key == "")
		return false;
	for (size_t i = 0; i < keyLen; i++)
		if (!std::isdigit(key[i]) && !std::isalpha(key[i])
				&& key[i] != '!' && key[i] != '#' && key[i] != '$'
				&& key[i] != '%' && key[i] != '&' && key[i] != '\''
				&& key[i] != '*' && key[i] != '+' && key[i] != '-'
				&& key[i] != '.' && key[i] != '^' && key[i] != '_'
				&& key[i] != '`' && key[i] != '|' && key[i] != '~')
			return false;
	return true;
}

bool isValidValue(const std::string &value) {
	size_t valueLen = value.length();

	for (size_t i = 0; i < valueLen; i++)
		if (value[i] < 9 || (value[i] > 9 && value[i] < 32) || value[i] > 126)
			return false;
	return true;
}

void setMalformedHeaderResponse(Response &response) {
	response.setMessage("Malformed header.");
	response.setCode(400);
}

bool Request::_addHeader(const std::string &headerLine) {
	std::stringstream	ss(headerLine);
	std::string			key;
	std::string			value;

	if (!std::getline(ss, key, ':'))
		return (setMalformedHeaderResponse(_response), false);
	std::getline(ss, value);

	if (!isValidKey(key) || _headers.find(key) != _headers.end())
		return (setMalformedHeaderResponse(_response), false);

	trim(value);
	toLower(key);
	if (!isValidValue(value))
		return (setMalformedHeaderResponse(_response), false);

	if (key == "content-length") {
		size_t valueLen = value.length();

		for (size_t i = 0; i < valueLen; i++)
			if (!std::isdigit(value[i]))
				return (setMalformedHeaderResponse(_response), false);
		if (valueLen > 10)
			return (setMalformedHeaderResponse(_response), false);
		_contentLength = std::atol(value.c_str());
		if (_contentLength > INT_MAX)
			return (setMalformedHeaderResponse(_response), false);

		if (_contentLength > _server.getClientMaxBodySize()) {
			_response.setMessage("Body sent is too large.");
			_response.setCode(413);
			return false;
		}
	}

	_headers[key] = value;
	return true;
}

void Request::handleRequest() {
	if (_location && std::find(_location->allowed_methods.begin(), _location->allowed_methods.end(), _method) == _location->allowed_methods.end()) {
		_response.setMessage("The requested HTTP method is not allowed.");
		_response.setCode(405);
	} else if (_method == "DELETE")
		_handleDelete();
	else if (_method == "POST")
		_handlePost();

	_response.createResponse();
}

void Request::_handleDelete() {
	if (std::remove(_targetFile.c_str()) != 0) {
		if (errno == ENOENT || errno == ENOTDIR || errno == EISDIR || errno == EFAULT) {
			_response.setCode(404);
			_response.setMessage("The file you want to delete does not exist.");
		} else if (errno == EACCES || errno == EPERM || errno == EROFS) {
			_response.setCode(403);
			_response.setMessage("You are not allowed to delete this file.");
		} else if (errno == EBUSY) {
			_response.setCode(423);
			_response.setMessage("File is locked or is currently in use.");
		} else {
			_response.setCode(500);
			_response.setMessage("File could not be deleted.");
		}
	}
}

void Request::_handlePost() {
	std::string extension;
	getFileExtension(_targetFile, extension);

	if (!extension.empty() && '.' + extension == _location->cgi_extension) {
		_cluster.executeCgi(*this);
	} else {
		_response.setCode(405);
		_response.setMessage("A POST request cannot be executed on something else than a CGI.");
	}
}

const std::string &Request::getMethod() const {
	return _method;
}

const std::string &Request::getTargetFile() const {
	return _targetFile;
}

const std::map<std::string, std::string> &Request::getHeaders() const {
	return _headers;
}

const std::string &Request::getBody() const {
	return _body;
}

const Response &Request::getResponse() const {
	return _response;
}

void Request::setResponseStr(const std::string &responseStr) {
	_response.setResponseStr(responseStr);
}

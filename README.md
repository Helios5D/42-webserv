# 42-webserv - by Hélios Daher and Maxime Rochedy

Welcome to our implementation of the **Webserv** project, completed as part of the 42 common core by **Maxime Rochedy** (mrochedy) and **Hélios Daher** (hdaher). Our project achieved a perfect score of **125/100** with bonus features including cookie management and support for two CGI systems (Python and PHP).

The goal of this project was to create a functional HTTP web server, following the **HTTP/1.1** protocol, that can handle client requests and responses according to the specifications of a basic web server. The project taught us network programming, concurrent handling of requests, and the fundamentals of web server architecture.

<img width="198" alt="125/100 grade" src="https://github.com/user-attachments/assets/708486c1-c045-40d6-ab0c-e5dc3650a50d">

## About the Project

The **Webserv** we implemented is a minimalist yet powerful web server, in line with **RFC 2616** (HTTP/1.1). The server is capable of managing multiple concurrent requests, serving static files, handling CGI scripts, and managing cookies to maintain session data.

### HTTP Methods
Our server supports the following HTTP methods:
- **GET** - Retrieve static files or, if targeting a CGI, execute it.
- **POST** - Execute a CGI, passing data to it.
- **DELETE** - Remove specified resources from the server.

### Concurrency
The server can handle multiple clients concurrently using non-blocking I/O and **epoll** for socket management, allowing efficient handling of numerous requests without blocking.

### CGI Support
CGI (Common Gateway Interface) support was implemented to allow dynamic content generation through scripts. We support **Python** and **PHP** CGIs.

This dual support enables the server to handle various content types and execute scripts dynamically.

### Cookie Management
As part of our bonus features, we implemented **cookie handling** to manage session data. Cookies are parsed from HTTP requests and can be used to **track client sessions**, enhancing the user experience and enabling persistent data across multiple requests.

## Getting Started

To get started with our **Webserv** project, clone the repository and build the project with the following commands:

```bash
git clone https://github.com/Helios5D/42-webserv.git
cd 42-webserv
make
```

### Running the Server

Once compiled, you can start the web server with a command like this:

```bash
./webserv [path_to_configuration_file]
```

If no configuration file is specified, the server will default to a built-in configuration.

### Configuration

The configuration file allows you to set up different servers and routes, mimicking the structure of an **Nginx configuration file**. Here, you can define:
- **Ports and IP addresses** to listen on
- **Root directories** for serving files
- **Error pages** for specific HTTP status codes
- **Server blocks** for defining multiple virtual hosts
- *and many other things...*

You can find an example configuration file at the root of the repository under the name `webserv.conf`.

## Contributing

If this repository was helpful, feel free to ⭐️ **star** ⭐️ it to boost visibility! If you would like to **contribute** to the Webserv project, feel free to fork the repository and create a pull request. Just as with other 42 projects, avoid submitting this code as your own.

Thank you for visiting, and happy coding!

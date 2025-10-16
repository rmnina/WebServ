# WebServ

WebServ is a project aimed at creating an infrastructure capable of **deploying and managing multiple web servers simultaneously**, each with its own configuration. Each server can receive and process **HTTP requests**.

<img width="759" height="205" alt="webserv1" src="https://github.com/user-attachments/assets/35e3bb62-2d50-4195-9210-7c8144d69de3" />


This project was carried out with the awesome [@eltouma](https://github.com/eltouma) and [@skiams](https://github.com/skiams).

## Project structure

The project has three main components:

### <ins>Configuration file management</ins>
Allows you to **define server settings**: ports, directories, authorized HTTP methods, request size limits, error handling, etc.
Configurations are stored in a structure (ConfigStruct) that manages keys and values, as well as **locations** (blocks specific to certain routes).

### <ins>Servers and their monitoring</ins>
Each server is represented by a class that contains:

- Configuration information, name, IP (hostname), and port.
- A server socket and an array of client sockets to manage connections.
- A buffer to store received requests.
- An epoll_event structure for tracking events on sockets.

Basically:
```
class Server
{
	private:		
		server_data					_config;
		location_data				_location;
		
		const std::string			_name;
		const std::string			_hostname;
		const std::string			_port;

		int							_server_socket;
		std::vector<int>			_client_sock;

		std::vector<std::string>	_request;

		struct addrinfo				*_info;
		struct epoll_event			_event;
	}
```

All servers are supervised by an **epoll instance**, which allows events on all sockets to be detected efficiently **(I/O multiplexing)**.

### <ins>HTTP parser</ins>
The parser is responsible for:
- Checking the validity of requests received according to the server configuration.
- Building HTTP responses with the correct status code, headers (MIME type, packet transfer, etc.) and content (body).
- Execute CGI (server-side scripts) for dynamic POST or GET requests.

## Supported HTTP requests

WebServ supports the main methods required for the project:
- <ins>**GET**</ins>: retrieve resources from the server.
- <ins>**POST**</ins>: send data to the server.
- <ins>**DELETE**</ins>: delete a resource on the server.

All connections are **persistent by default** (the server remains listening for new requests).

## General operation

A **client** sends an **HTTP request** to the **server**.

The server **accepts** the connection, **duplicates the socket** for the client, and **stores the request**.

The parser **analyzes** the request and **constructs the appropriate response**.

The **response** is **sent to the client**, possibly in several packets thanks to **TCP/IP** and **I/O multiplexing**.

**The epoll instance monitors all servers and clients to handle events simultaneously**.

## Technical notes

- <ins>**Sockets**</ins> allow data to pass between the server and client via TCP/IP.
- <ins>**Locations**</ins> in the configuration file allow specific rules to be defined for certain routes without impacting the rest of the server.
- <ins>**Parsing**</ins> is based on std::vector and std::map for flexible manipulation of keys/values and configuration blocks.

Here is an outline of the project structure :
<img width="2645" height="2366" alt="webserv2" src="https://github.com/user-attachments/assets/0d06b007-3840-4868-a69b-42776c7ba56f" />

## Websites

The servers are programmed to run any static website. They can also execute CGI scripts. Below is the website we have provided to demonstrate their functionality :
*image to come*

## Use of the project

```
git clone https://github.com/rmnina/WebServ && cd Webserv
make
./webserv [config file]
```



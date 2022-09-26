#include "Server.hpp"

Server::Server(int const & port, char const * password) : _port(port), _password(password) {}

Server::Server(Server const & copy) : _hostname(copy._hostname) {}

Server & Server::operator=(Server const & assign)
{
	this->~Server();
	new (this) Server(assign);
	return (*this);
}

Server::~Server(void)
{
	std::cout << "Server cleaned up" << std::endl;
}

void    Server::init_listener(void)
{
	_server_address.sin_family = AF_INET;
	_server_address.sin_addr.s_addr = INADDR_ANY;
	_server_address.sin_port = htons(_port);
	sockfd = socket(_server_address.sin_family, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cout << "Error on socket(): " << errno << std::endl;
		exit(1);
	}
	client_addr_len = sizeof(client_addr);
	std::cout << "Socket = [" << sockfd << "]" << std::endl;

	if (bind(sockfd, (struct sockaddr *) &_server_address, sizeof(_server_address)) < 0)
	{
		std::cout << "Error on bind(): " << errno << std::endl;
		close(sockfd);
		exit(1);
	}

	if (listen(sockfd, 5) < 0)
	{
		std::cout << "Error on listen(): " << errno << std::endl;
		exit(1);
	}

	std::memset(&client_addr, 0, sizeof(client_addr));
}

void    Server::start_server(void)
{
	int		new_fd;
	char	buf[256]; // buffer for client data (cstring..?)
	char	remote_ip[INET6_ADDRSTRLEN];

	clients[0].fd = sockfd; // adds listening socket to first client position to loop
	clients[0].events = POLLIN; // reports ready to read on incoming connection
	fd_count = 1; // for the listener
	while (true)
	{
		std::cout << "polling" << std::endl;
		if (poll(clients, fd_count, -1) < 0)
		{
			std::cout << "Error on poll(): " << errno << std::endl;
			exit(1);
		}
		for (int i = 0; i < fd_count; i++)
		{
			if (clients[i].revents & POLLIN) // if someone is ready to read (be read?)
			{
				if (clients[i].fd == sockfd) // if its listener, need to handle new connection
				{
					client_addr_len = sizeof(client_addr);
					new_fd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
					if (new_fd < 0)
					{
						std::cout << "Error on accept(): " << errno << std::endl;
						exit(1);
					}
					else
					{
						add_new_client(new_fd, &fd_count); // original passes fd_size for realloc, we use container so don't need...?
						std::cout << "pollserver: new connection from " 
							<< inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr*)&client_addr), remote_ip, INET6_ADDRSTRLEN)
							<< " on socket "
							<< new_fd
							<< std::endl;
					}
				}
				else // If not the listener, we're just a regular client (already accepted in previous condition)
				{
					int n_bytes = recv(clients[i].fd, buf, 256 - 1, 0);
					std::cout << "n of bytes recieved: [" << n_bytes << "]" << std::endl;
					int sender_fd = clients[i].fd;

					if (n_bytes <= 0)
					{
						if (n_bytes == 0) // connection is closed
						{
							std::cout << "pollserver: socket "
								<< sender_fd
								<< " hung up"
								<< std::endl;
						}
						else
						{
							std::cout << "Error on recv(): " << errno << std::endl;
							exit(1);
						}
						close(clients[i].fd);
						remove_client(i, &fd_count);
					}
					else // we got something valid from a client
					{
						
						Message recieved(clients[i].fd, buf);
						recieved.parse();
						memset(buf, 0, sizeof(buf));

						for (int j = 0; j < fd_count; j++)
						{
							// starts collecting destinations to send the data out
							int dest_fd = clients[j].fd;

							// if (dest_fd != sockfd && dest_fd != sender_fd)
							if (dest_fd != sockfd) // sends message back
							{
								if (send(dest_fd, buf, n_bytes, 0) < 0)
								{
									std::cout << "Error on send(): " << errno << std::endl;
									exit(1);
								}
							}
						}
					}
				}
			}
		}
	}
}

void	Server::add_new_client(int newfd, int *fd_count)
{
	// no realloc to handle more than 64 clients
	clients[*fd_count].fd = newfd;
	clients[*fd_count].events = POLLIN;
	(*fd_count)++;
}

void	Server::remove_client(int i, int *fd_count)
{
	// Copy the one from the end over this one
	clients[i] = clients[*fd_count - 1];
	(*fd_count)--;
}

// Get sockaddr, IPv4 or IPv6:
void * Server::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
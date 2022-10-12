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

	int yes=1;
	// lose the pesky "Address already in use" error message
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}

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
						parse_messages(clients[i].fd, buf);
						exec_cmds();
						memset(buf, 0, sizeof(buf));
						size_t queue_len = send_msg_queue.size();
						for (size_t j = 0; j < queue_len; j++)
						{
							// check if message is for channel
							Message msg_to_send = send_msg_queue.front();
							if (msg_to_send.receiver_is_channel())
							{
								std::map<std::string, Channel>::iterator channel = channel_list.find(msg_to_send.get_receiver());
								if (channel != channel_list.end())
									send_channel_msg(msg_to_send, channel->second);
							}
							else
							{
								send_priv_msg(msg_to_send);
							}
							send_msg_queue.pop();
						}
					}
				}
			}
		}
	}
}

void	Server::parse_messages(int const &fd, char *buf)
{
	char	*line;
	char	*end_str;
	int		idx;
	std::map<int, Client>::iterator	it;
	std::string	sender = "";
	//
	// if fd matches a stored fd, get the name from map and assign it to sender 
	for (it = client_list.begin(); it != client_list.end(); it++)
	{
		// if the nickname is equal to the arg, and the fd already exists, change it
		if (it->first == fd)
		{
			std::cout << "User is already registered and sender can be assigned" << std::endl;
			sender = it->second.get_nickname();
		}
	}

	// std::cout << "msg to parse: " << _raw << std::endl;
	line = strtok_r(buf, "\n", &end_str);
	while (line != NULL)
	{
		Message *temp = new Message(fd);
		char *sgmt;
		char *end_token;
		std::cout << "line: " << line << std::endl;
		sgmt = strtok_r(line, " \r\n", &end_token);
		idx = -1;
		while (sgmt != NULL)
		{
			idx++;
			// need to do other checks etc
			if (idx == 0) // need to fix for every line
				temp->set_cmd(sgmt);
			if (idx == 1)
			{
				temp->set_arg(sgmt);
			}
			std::cout << idx << ":#\tsep sgmt: " << sgmt << std::endl;
			sgmt = strtok_r(NULL, " \r\n", &end_token);
		}
		temp->set_sender(sender);
		received_msg_queue.push(temp);
		line = strtok_r(NULL, " \r\n", &end_str);
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

void Server::print_error(const int &client_fd, std::string error_msg)
{
	(void)client_fd;
	std::cout << "Error: " << error_msg << std::endl;
}

void	Server::send_priv_msg(Message const &msg)
{
	ssize_t bytes_sent = 0;

	std::cout << "SENDING: " << msg.raw_msg() << std::endl;
	bytes_sent = send(msg.get_fd(), msg.raw_msg(), msg.msg_len(), MSG_DONTWAIT);
	if (bytes_sent < (ssize_t)msg.msg_len())
	{
		std::cout << "error: message was not fully sent" << std::endl;
	}
}

void	Server::send_channel_msg(Message const &msg, Channel const &channel)
{
	std::map<int, std::string>::const_iterator itb = channel.get_member_list().begin();
	std::map<int, std::string>::const_iterator ite = channel.get_member_list().end();
	ssize_t bytes_sent = 0;
	int	sender = msg.get_fd();
	
	for (; itb != ite; ++itb)
	{
		std::cout << "SENDING: [" << msg.raw_msg() << "] to fd:[" << msg.get_fd() << "]" << std::endl;
		bytes_sent = send(msg.get_fd(), msg.raw_msg(), msg.msg_len(), MSG_DONTWAIT);
		if (itb->first != sender)
		{
			bytes_sent = send(itb->first, msg.raw_msg(), msg.msg_len(), MSG_DONTWAIT);
		}
	}
	if (bytes_sent < (ssize_t)msg.msg_len())
	{
		// return error that message was not fully sent
	}
}

exec_funcs Server::match_cmd(std::string cmd)
{
	std::string cmds[] = {"NICK", "LUSERS"};
	exec_funcs	func_pointers[] = {
		&Server::exec_cmd_NICK,
		&Server::exec_cmd_LUSERS
	};

	for (int i = 0; i < 2; i++)
	{
		if (cmds[i] == cmd)
			return (this->*func_pointers[i])();
	}

	std::cout << "CMD:" << cmd_msg->get_cmd() << std::endl;
	// if (cmd_msg->get_cmd() == "NICK")
		// return (NICK);
	return (void);
}


int	Server::exec_cmds()
{

	for (int i = 0; i < (int)received_msg_queue.size(); i++)
	{
		Message *cmd_msg = received_msg_queue.front();
		ServerCMD cmd = match_cmd(cmd_msg);


		delete cmd_msg;
		received_msg_queue.pop();
	}
	return (0);
}
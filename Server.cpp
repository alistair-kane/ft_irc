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
						// if the fd is marked as registered we parse message as normal
						// otherwise the registration handler must be invoked
						parse_messages(clients[i].fd, buf);

						std::cout << "NUM ELEMENTS: [" << numElements << std::endl;
						int *reg_client_end = reg_clients + numElements;
						if (std::find(reg_clients, reg_client_end, clients[i].fd) != reg_client_end)
						{
							// client is already registered
							exec_cmds();
						}
						else
							handle_registration();
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
		idx = 0;
		while (sgmt != NULL)
		{
			// need to do other checks etc
			if (idx == 0) // need to fix for every line
				temp->set_cmd(sgmt);
			if (idx >= 1)
			{
				temp->set_arg(idx, sgmt);
			}
			// std::cout << idx << ":#\tsep sgmt: " << sgmt << std::endl;
			sgmt = strtok_r(NULL, " \r\n", &end_token);
			idx++;
		}
		temp->set_sender(sender);
		received_msg_queue.push(temp);
		line = strtok_r(NULL, " \r\n", &end_str);
	}
}

void	Server::push_msg(int fd, std::string text)
{
	std::string	full;

	full = text + "\r\n";
	Message msg(fd, full);
	send_msg_queue.push(msg);
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

Client *Server::get_client(int const & fd)
{
	std::map<int, Client>::iterator client_it = client_list.find(fd);
	if (client_it != client_list.end())
		return (&client_it->second);
	return (NULL);
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

Message &Server::reg_parser(Message *msg, int &fd, std::string &cmd, std::string &arg, std::string &sender) 
{
	Message &msg_copy = *msg;
	delete msg;
	fd = msg_copy.get_fd();
	cmd = msg_copy.get_cmd();
	arg = msg_copy.get_arg(0);
	sender = msg_copy.get_sender();
	return (msg_copy);
}

void Server::handle_registration(void)
{
	std::string nick;
	std::string user;
	std::string realname;
	
	int			fd;
	std::string	cmd;
	std::string	arg;
	std::string sender;

	reg_parser(received_msg_queue.front(), fd, cmd, arg, sender);
	
	// Message *msg = received_msg_queue.front();
	// Message msg_copy = *msg;
	// delete msg;
	// std::string cmd = msg_copy.get_cmd();
	// std::string arg = msg_copy.get_arg(0);
	// int			fd = msg_copy.get_fd();

	// password must be set in config file!!!!
	if (cmd == "PASS")
	{
		if (arg == "")
			reply_461(fd, "PASS", sender); // not enough args
		else if (arg == _password)
		{
			received_msg_queue.pop();
			reg_parser(received_msg_queue.front(), fd, cmd, arg, sender);

			// Message *msg = received_msg_queue.front();
			// Message msg_copy = *msg;
			// delete msg;
			// std::string cmd = msg_copy.get_cmd();
			// std::string arg = msg_copy.get_arg(0);
			// int			fd = msg_copy.get_fd();

			if (cmd == "NICK")
			{
				if (arg == "")
				{
					push_msg(fd, "431 :No nickname provided");
					return ;
				}
				if (check_nickname(arg) == true)
					push_msg(fd, ("433 * " + arg + " :Nickname already taken"));
				else
					nick = arg;
			}
			else if (cmd == "QUIT")
			{
				std::cout << "Client quit" << std::endl;
				return ;
			}

			received_msg_queue.pop();
			Message &msg = reg_parser(received_msg_queue.front(), fd, cmd, arg, sender);

			// Message *cmd_msg = received_msg_queue.front();
			// Message msg_copy = *msg;
			// delete msg;
			// std::string cmd = msg_copy.get_cmd();
			// std::string arg = msg_copy.get_arg(0);
			// int			fd = msg_copy.get_fd();
			if (cmd == "USER")
			{
				if (msg.get_arg(3).empty()) // probably wrong, need max filled idx or some bs
				{
					reply_461(fd, cmd, nick);
					return ;
				}
				user = arg;
				realname = msg.get_arg(3);
			}
			else if (cmd == "QUIT")
			{
				std::cout << "Client quit" << std::endl;
				return ;
			}

			// if both nick and user is set proceed to client registration
			if (!nick.empty() && !user.empty())
			{
				Client new_client(fd, nick);
				client_list.insert(std::make_pair(fd, new_client));

				msg.set_sender(new_client.get_nickname());
				// send a message back?
				push_msg(fd, ("001 " + new_client.get_nickname() + " :Hi, welcome to IRC"));
				push_msg(fd, ("002 " + new_client.get_nickname() + " :Your host is " +
					_hostname + ", running version ALISTIM-v0.01"));
				// could generate a timestamp when server class initialized to use here
				push_msg(fd, ("003 " + new_client.get_nickname() + " :This server was created 2022AD"));
				push_msg(fd, ("004 " + new_client.get_nickname() + " " + _hostname + "ALISTIM-v0.01 o o"));
				exec_cmd_LUSERS(msg);
				exec_cmd_MOTD(msg);
			}
		}
		else
			push_msg(fd, "464 :Password incorrect");
	}
}

void Server::match_cmd(Message &msg)
{
	int const size = 6;
	std::string cmds[] = {"JOIN", "LUSERS", "MOTD", "NICK", "PING", "PONG"};
	void (Server::*func_pointers[size])(Message &msg) = {
		&Server::exec_cmd_JOIN,
		&Server::exec_cmd_LUSERS,
		&Server::exec_cmd_MOTD,
		&Server::exec_cmd_NICK,
		&Server::exec_cmd_PING,
		&Server::exec_cmd_PONG
	};
	for (int i = 0; i < size; i++)
	{
		if (cmds[i] == msg.get_cmd())
			(this->*func_pointers[i])(msg);
	}
}

int	Server::exec_cmds()
{
	for (int i = 0; i < (int)received_msg_queue.size(); i++)
	{
		Message *cmd_msg = received_msg_queue.front();

		match_cmd(*cmd_msg);
		delete cmd_msg;
		received_msg_queue.pop();
	}
	return (0);
}

void	Server::reply_461(int fd, std::string cmd, std::string nick)
{
	push_msg(fd, ("461 " + nick + " " + cmd + " :Not enough parameters"));
}

bool	Server::check_nickname(std::string arg)
{
	std::map<int, Client>::iterator	it;
	for (it = client_list.begin(); it != client_list.end(); it++)
	{
		std::string	current = it->second.get_nickname();
		if (current == arg)
			return (true);
	}
	return (false);
}

// void	Server::disconnect(int fd, std::string msg)
// {
// 	push_msg(fd, ("ERROR :" + msg))
// 	std::cout << "Disconnected connection from"
// }
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

	int yes=1;
	// lose the pesky "Address already in use" error message
	if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1)
	{
		perror("setsockopt");
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
						memset(buf, 0, sizeof(buf));
						exec_cmds();
						for (int j = 0; j < (int)send_msg_queue.size(); j++)
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
	// std::cout << "msg to parse: " << _raw << std::endl;
	line = strtok_r(buf, "\n", &end_str);
	while (line != NULL)
	{
		Message temp(fd);
		char *sgmt;
		char *end_token;
		std::cout << "line: " << line << std::endl;	
		sgmt = strtok_r(line, " ", &end_token);
		idx = 0;
		while (sgmt != NULL)
		{
			// need to do other checks etc
			if (idx == 0) // need to fix for every line
				temp.set_cmd(sgmt);
			else if (idx == 1)
				temp.set_arg(sgmt);
			std::cout << idx << ":#\tsep sgmt: " << sgmt << std::endl;
			sgmt = strtok_r(NULL, " ", &end_token);
			idx++;
		}
		received_msg_queue.push(temp);
		line = strtok_r(NULL, "\n", &end_str);
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

	std::cout << "SEND IT: " << msg.raw_msg() << std::endl;
	bytes_sent = send(msg.get_fd(), msg.raw_msg(), msg.msg_len(), MSG_DONTWAIT);
	if (bytes_sent < (ssize_t)msg.msg_len())
	{
		// return error that message was not fully sent
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

Server::ServerCMD Server::match_cmd(Message &cmd_msg)
{
	std::cout << "CMD:" << cmd_msg.get_cmd() << std::endl;
	if (cmd_msg.get_cmd() == "NICK")
		return (NICK);
	return (WHOWAS);
}


int	Server::exec_cmds()
{
	for (int i = 0; i < (int)received_msg_queue.size(); i++)
	{
		Message cmd_msg = received_msg_queue.front();
		ServerCMD cmd = match_cmd(cmd_msg);
		switch (cmd)
		{
			case ADMIN:
				exec_cmd_ADMIN(cmd_msg);
				break;
			case AWAY:
				exec_cmd_AWAY(cmd_msg);
				break;
			case INVITE:
				exec_cmd_INVITE(cmd_msg);
				break;
			case JOIN:
				exec_cmd_JOIN(cmd_msg);
				break;
			case KICK:
				exec_cmd_KICK(cmd_msg);
				break;
			case KNOCK:
				exec_cmd_KNOCK(cmd_msg);
				break;
			case LINKS:
				exec_cmd_LINKS(cmd_msg);
				break;
			case LIST:
				exec_cmd_LIST(cmd_msg);
				break;
			case LUSERS:
				exec_cmd_LUSERS(cmd_msg);
				break;
			case MAP:
				exec_cmd_MAP(cmd_msg);
				break;
			case MODE:
				exec_cmd_MODE(cmd_msg);
				break;
			case MOTD:
				exec_cmd_MOTD(cmd_msg);
				break;
			case NAMES:
				exec_cmd_NAMES(cmd_msg);
				break;
			case NICK:
				exec_cmd_NICK(cmd_msg);
				break;
			case NOTICE:
				exec_cmd_NOTICE(cmd_msg);
				break;
			case PART:
				exec_cmd_PART(cmd_msg);
				break;
			case PASS:
				exec_cmd_PASS(cmd_msg);
				break;
			case PING:
				exec_cmd_PING(cmd_msg);
				break;
			case PONG:
				exec_cmd_PONG(cmd_msg);
				break;
			case PRIVMSG:
				exec_cmd_PRIVMSG(cmd_msg);
				break;
			case QUIT:
				exec_cmd_QUIT(cmd_msg);
				break;
			case RULES:
				exec_cmd_RULES(cmd_msg);
				break;
			case SETNAME:
				exec_cmd_SETNAME(cmd_msg);
				break;
			case SILENCE:
				exec_cmd_SILENCE(cmd_msg);
				break;
			case STATS:
				exec_cmd_STATS(cmd_msg);
				break;
			case USER:
				exec_cmd_USER(cmd_msg);
				break;
			case VERSION:
				exec_cmd_VERSION(cmd_msg);
				break;
			case WHO:
				exec_cmd_WHO(cmd_msg);
				break;
			case WHOIS:
				exec_cmd_WHOIS(cmd_msg);
				break;
			case WHOWAS:
				exec_cmd_WHOWAS(cmd_msg);
				break;
			default:
				break;
		}
		received_msg_queue.pop();
	}
	return (0);
}
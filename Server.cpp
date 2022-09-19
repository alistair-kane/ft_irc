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

void    Server::init_server(void)
{
    _server_address.sin_family = AF_INET;
    _server_address.sin_addr.s_addr = INADDR_ANY;
    _server_address.sin_port = htons(_port);
    sockfd = socket(_server_address.sin_family, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cout << "Error: " << errno << std::endl;
		exit(1);
	}
    client_addr_len = sizeof(client_addr);
    std::cout << "Socket = [" << sockfd << "[" << std::endl;

    if (bind(sockfd, (struct sockaddr *) &_server_address, sizeof(_server_address)) < 0)
    {
        std::cout << "Error: " << errno << std::endl;
        exit(1);
    }

    if (listen(sockfd, 5) < 0)
	{
		std::cout << "Error: " << errno << std::endl;
		exit(1);
    }

    std::memset(&client_addr, 0, sizeof(client_addr));
}

void    Server::start_server(void)
{
    int client_fd;
    int return_poll;

    clients_size = 0;
    while (true)
    {
        client_fd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
		if (client_fd < 0)
		{
			std::cout << "Error: " << errno << std::endl;
        	exit(1);
		}
        if (client_fd > 0)
        {
            std::cout << "Accept = [" << client_fd << "] client = [" << client_addr.ss_family << std::endl;
            add_new_client(client_fd);
        }
        return_poll = poll(clients, clients_size, 500);
        if (return_poll > 0)
		{
			std::cout << "... collect messages\n";
			collect_messages();
			// std::cout << "... process messages\n";
			// process_messages();
			// std::cout << "... distribute messages\n";
			// distribute_messages();
			std::cout << "... wait for new messages\n";
		}
        if (clients_size != static_cast<int>(client_list.size()))
            update_pollfd();
    }
}

void	Server::collect_messages(void)
{
	char buffer[1024] = {0};
	ssize_t returnRecv = 0;

	for (int i = 0; i < clients_size; ++i)
	{
		if ((clients[i].revents & POLLIN) == POLLIN)
		{
			returnRecv = recv(clients[i].fd, buffer, 1023, 0);
			buffer[returnRecv] = '\0';
			//check for "\r\n" to mark end of the message otherwise append
			if (returnRecv == 0)
			{
				this->remove_client(clients[i].fd);
				close(clients[i].fd);
				std::cout << "client disconnected\n";
				continue;
			}
			std::cout << "recv = " << returnRecv << " from fd = " << clients[i].fd << " buffer = [" << buffer << "]\n";
			this->store_message(clients[i].fd, buffer);
		}
	}
}

void	Server::update_pollfd(void)
{
	std::map<int, Client>::iterator it = client_list.begin();
	clients_size = static_cast<int>(client_list.size());
	for (int i = 0; i < clients_size; ++i)
	{
		clients[i].fd = it->first;
		clients[i].events = POLLIN;
		clients[i].revents = 0;
	}
}

void	Server::add_new_client(int const & fd)
{
	client_list.insert(std::make_pair(fd, Client(fd)));
	//++this->clients_size;
	//this->update_pollfd();
}

void	Server::remove_client(int const & fd)
{
	client_list.erase(fd);
	//--this->clients_size;
	//this->update_pollfd();
}

void	Server::store_message(int const & fd, char const * input)
{
	std::string tmp = input;
	size_t start_pos = 0, end_pos = 0;
	std::map<int, Client>::iterator it = client_list.find(fd);
	do
	{
		end_pos = tmp.find("\r\n", start_pos);
		if (end_pos == std::string::npos) //incomplete message
			it->second.append_message_buffer(input);
		else
		{
			it->second.append_message_buffer(tmp.substr(start_pos, end_pos));
			received_msg_queue.push(Message(fd, it->second.get_message_buffer().c_str()));
			it->second.clear_message_buffer();
			start_pos = end_pos + 2;
		}
	} while (end_pos != std::string::npos);

	this->recieved_msg_list.insert(std::make_pair(fd, Message(fd, input)));
}

void	Server::remove_message(int const & fd)
{
	this->recieved_msg_list.erase(fd);
}

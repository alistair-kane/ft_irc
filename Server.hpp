#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>

#include <errno.h>
#include <stdlib.h>
// #include <vector>

#include <cstring>


#include <map>
#include <queue>

#include "Message.hpp"
#include "Client.hpp"

class Server
{
	public:
		enum ServerCMD
		{
			ADMIN,
			AWAY,
			INVITE,
			JOIN,
			KICK,
			KNOCK,
			LINKS,
			LIST,
			LUSERS,
			MAP,
			MODE,
			MOTD,
			NAMES,
			NICK,
			NOTICE,
			PART,
			PASS,
			PING,
			PONG,
			PRIVMSG,
			QUIT,
			RULES,
			SETNAME,
			SILENCE,
			STATS,
			USER,
			VERSION,
			WHO,
			WHOIS,
			WHOWAS
		};

		Server(int const & port, char const * password);
		Server(Server const & copy);
		Server & operator=(Server const & assign);
		~Server(void);

		void    init_listener(void);
		void    start_server(void);
		int		exec_cmds(ServerCMD cmd, char **cmd_args);

	private:

		struct sockaddr_in		_server_address;
		int						_port;
		std::string				_password; // could go in conf file later?
		std::string				_hostname;

		//place to store the clients
		std::map<int, Client>	client_list;

		// need to create message class?
		std::queue<Message>	received_msg_queue;
		std::queue<Message>	send_msg_queue;
		std::map<int, Message>	recieved_msg_list;
		std::map<int, Message>	sent_msg_list;

		// need to create channel class?
		// std::map<std::string, Channel>	channel_list;
		struct pollfd			clients[64];
		int						sockfd;
		struct sockaddr_storage	client_addr;
		socklen_t				client_addr_len;
		int						fd_count;

		// void					collect_messages(void);
		// void					update_pollfd(void);
		// int						get_client_fd(std::string const & nickname);
		
		void					add_new_client(int newfd, int *fd_count);
		void					remove_client(int i, int *ft_count);

		void *					get_in_addr(struct sockaddr *sa);

		// void					store_message(int const & fd, char const * input);
		// void					remove_message(int const & fd);

};
#pragma once 

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

#include <cstdio>
#include <sstream>

#include <errno.h>
#include <stdlib.h>
// #include <vector>
#include <algorithm>
#include <cstring>


#include <map>
#include <queue>

#include "Message.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Server
{
	public:
		enum ServerCMD
		{
			ADMIN,
			AWAY,
			BAN,
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
		int		exec_cmds(void);
		void	push_msg(int fd, std::string text);

	private:

		struct sockaddr_in		_server_address;
		int						_port;
		std::string				_password = "test"; // could go in conf file later?
		std::string				_hostname;

		//place to store the clients
		std::map<int, Client>			client_list;
		std::map<std::string, Channel>	channel_list;

		// need to create message class?
		std::queue<Message *>	received_msg_queue;
		std::queue<Message>		send_msg_queue;
		std::map<int, Message>	recieved_msg_list;
		std::map<int, Message>	sent_msg_list;

		// need to create channel class?
		// std::map<std::string, Channel>	channel_list;
		struct pollfd			clients[64];
		std::vector<int>		auth_clients;
		std::vector<int>		reg_clients;
		// int						reg_count;

		int						sockfd;
		struct sockaddr_storage	client_addr;
		socklen_t				client_addr_len;
		int						fd_count;

		// void					collect_messages(void);
		// void					update_pollfd(void);
		// int						get_client_fd(std::string const & nickname);
		Client					*get_client(int const & fd);
		
		void					add_new_client(int newfd, int *fd_count);
		void					remove_client(int i, int *ft_count);

		void *					get_in_addr(struct sockaddr *sa);

		// void					store_message(int const & fd, char const * input);
		// void					remove_message(int const & fd);
		void					parse_messages(int const &fd, char *buf);
		bool					handle_pass(void);
		void					register_client(Message &msg, int fd, std::string nick);
		void					handle_registration(void);
		void					match_cmd(Message &msg);
		// exec_funcs				match_cmd(std::string cmd);

		// When we created the Channel class we will use this function to distribute messages
		// void					send_msg(Message const &msg, Channel const &channel);
		void	send_priv_msg(Message const &msg);
		void	send_channel_msg(Message const &msg, Channel const &channel);

		void	print_error(const int &client_fd, std::string error_msg);
		void	reply_461(int fd, std::string cmd, std::string nick);
		// void	disconnect(int fd, std::string msg);
		Message	&reg_parser(Message *msg, int &fd, std::string &cmd, std::string &arg, std::string &sender);
		bool	check_nickname(std::string arg);
		


		// Server command functions
		void	exec_cmd_ADMIN(Message &cmd_msg);
		void	exec_cmd_AWAY(Message &cmd_msg);
		void	exec_cmd_BAN(Message &cmd_msg);
		void	exec_cmd_INVITE(Message &cmd_msg);
		void	exec_cmd_JOIN(Message &cmd_msg);
		void	exec_cmd_KICK(Message &cmd_msg);
		void	exec_cmd_KNOCK(Message &cmd_msg);
		void	exec_cmd_LINKS(Message &cmd_msg);
		void	exec_cmd_LIST(Message &cmd_msg);
		void	exec_cmd_LUSERS(Message &cmd_msg);
		void	exec_cmd_MAP(Message &cmd_msg);
		void	exec_cmd_MODE(Message &cmd_msg);
		void	exec_cmd_MOTD(Message &cmd_msg);
		void	exec_cmd_NAMES(Message &cmd_msg);
		void	exec_cmd_NICK(Message &cmd_msg);
		void	exec_cmd_NOTICE(Message &cmd_msg);
		void	exec_cmd_PART(Message &cmd_msg);
		void	exec_cmd_PASS(Message &cmd_msg);
		void	exec_cmd_PING(Message &cmd_msg);
		void	exec_cmd_PONG(Message &cmd_msg);
		void	exec_cmd_PRIVMSG(Message &cmd_msg);
		void	exec_cmd_QUIT(Message &cmd_msg);
		void	exec_cmd_RULES(Message &cmd_msg);
		void	exec_cmd_SETNAME(Message &cmd_msg);
		void	exec_cmd_SILENCE(Message &cmd_msg);
		void	exec_cmd_STATS(Message &cmd_msg);
		void	exec_cmd_USER(Message &cmd_msg);
		void	exec_cmd_VERSION(Message &cmd_msg);
		void	exec_cmd_WHO(Message &cmd_msg);
		void	exec_cmd_WHOIS(Message &cmd_msg);
		void	exec_cmd_WHOWAS(Message &cmd_msg);

};

typedef void	(Server:: *exec_funcs)(Message &msg);
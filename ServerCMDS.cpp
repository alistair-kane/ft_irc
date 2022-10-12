#include "Server.hpp"

// int	Server::exec_cmd_ADMIN(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_AWAY(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_INVITE(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

void	Server::exec_cmd_JOIN(Message &cmd_msg)
{
	// get fd from user joining channel
	int	const fd = cmd_msg.get_fd();

	// get the channel name from the argument
	std::string	const &channel_name = cmd_msg.get_arg();

	// get client who wants to join channel by fd
	Client *client_to_add = get_client(fd);
	if (client_to_add == NULL)
		return ;

	// get nickname of client
	std::string nick = client_to_add->get_nickname();

	std::map<std::string, Channel>::iterator channel = channel_list.find(channel_name);

	// if channel doesn't exist create one with caller as operator/channel_owner
	if (channel == channel_list.end())
	{
		channel_list.insert(std::make_pair(channel_name, Channel(channel_name, fd)));
		channel = channel_list.find(channel_name);
	}

	// if channel exists add user to it
	channel->second.add_member(fd, nick);

	return ;
}

// int	Server::exec_cmd_KICK(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_KNOCK(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_LINKS(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_LIST(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

void	Server::exec_cmd_LUSERS(Message &cmd_msg)
{
	Message msg(cmd_msg.get_fd(), ("251 " + cmd_msg.get_sender() + " :There are " +
		std::to_string(client_list.size()) + " users and 0 services on 1 server\r\n"));
	send_msg_queue.push(msg);
	return ;
}

// int	Server::exec_cmd_MAP(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_MODE(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_MOTD(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_NAMES(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// registering the session
// !!!! need to handle changing the nickname
void	Server::exec_cmd_NICK(Message &cmd_msg)
{
	std::map<int, Client>::iterator	it;

	// check if the arg isnt empty or invalid

	// check if the param of nick is already contained in the client list
	//		if yes - return the error
	for (it = client_list.begin(); it != client_list.end(); it++)
	{
		// if the nickname is equal to the arg, and the fd already exists, change it
		if (it->second.get_nickname() == cmd_msg.get_arg())
		{
			std::cout << "Name already exists error etc" << std::endl;
			return ;
		}
	}
	// if not, create a new instance of the client class and add to the client list map
	// along with the fd of the socket 
	Client new_client(cmd_msg.get_fd(), cmd_msg.get_arg());
	client_list.insert(std::make_pair(cmd_msg.get_fd(), new_client));
	cmd_msg.set_sender(new_client.get_nickname());
	// send a message back?
	Message msg1(cmd_msg.get_fd(), ("001 " + new_client.get_nickname() + " :Hi, welcome to IRC\r\n"));
	send_msg_queue.push(msg1);
	Message msg2(cmd_msg.get_fd(), ("002 " + new_client.get_nickname() + " :Your host is " +
		_hostname + ", running version ALISTIM-v0.01 \r\n"));
	send_msg_queue.push(msg2);
	// could generate a timestamp when server class initialized to use here
	Message msg3(cmd_msg.get_fd(), ("003 " + new_client.get_nickname() + " :This server was created 2022AD\r\n"));
	send_msg_queue.push(msg3);
	Message msg4(cmd_msg.get_fd(), ("004 " + new_client.get_nickname() + " " + _hostname + "ALISTIM-v0.01 o o\r\n"));
	send_msg_queue.push(msg4);
	exec_cmd_LUSERS(cmd_msg);
	
	// return (0);
}

// int	Server::exec_cmd_NOTICE(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_PART(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_PASS(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_PING(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_PONG(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_PRIVMSG(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_QUIT(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_RULES(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_SETNAME(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_SILENCE(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_STATS(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_USER(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_VERSION(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_WHO(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_WHOIS(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

// int	Server::exec_cmd_WHOWAS(Message &cmd_msg)
// {
// 	(void)cmd_msg;
// 	return (0);
// }

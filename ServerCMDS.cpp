#include "Server.hpp"

void	Server::exec_cmd_ADMIN(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_AWAY(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_BAN(Message &cmd_msg)
{
	// check if user is operator by iterating over operator list and check if msg fd is in that list
	// get fd of requesting user
	int const request_fd = cmd_msg.get_fd();

	// get channel the user should be banned from
	std::string	const &channel_name = cmd_msg.get_arg(0);
	std::map<std::string, Channel>::iterator channel = channel_list.find(channel_name);

	// get the list of operators of that channel
	std::set<int> operator_list = channel->second.get_operator_list();
	std::set<int>::iterator iter;
	bool is_operator = false;
   
	// Loop over the operator_list
	for (iter = operator_list.begin(); iter != operator_list.end(); iter++)
	{
		if (request_fd == *iter)
		{
			is_operator = true;
		}
	}

	if (is_operator)
	{
		// find user to ban in member_list from msg_arg

		// if user to ban is not in member_list then throw error user not found

		// if user to ban is in member_list remove from member_list and create message that user was banned

		// add user to ban list 
	}

	return ;
}

void	Server::exec_cmd_INVITE(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_JOIN(Message &cmd_msg)
{
	// get fd from user joining channel
	int	const fd = cmd_msg.get_fd();

	// get the channel name from the argument
	std::string	const &channel_name = cmd_msg.get_arg(0);

	// get client who wants to join channel by fd
	Client *client_to_add = get_client(fd);
	if (client_to_add == NULL)
		return ;

	// get nickname of client
	std::string nick = client_to_add->get_nickname();



	std::map<std::string, Channel>::iterator channel = channel_list.find(channel_name);

	std::set<std::string> ban_list = channel->second.get_ban_list();
	std::set<std::string>::iterator iter;
	bool is_banned = false;
   
	// Loop over the operator_list
	for (iter = ban_list.begin(); iter != ban_list.end(); iter++)
	{
		if (nick == *iter)
		{
			is_banned = true;
		}
	}

	if (is_banned)
	{
		// throw error that user is banned
		return ;
	}

	// if channel doesn't exist create one with caller as operator/channel_owner
	if (channel == channel_list.end())
	{
		channel_list.insert(std::make_pair(channel_name, Channel(channel_name, fd)));
		channel = channel_list.find(channel_name);
		channel->second.add_operator(fd);

		// Create initial channel message
		std::string join_channel_msg = client_to_add->get_nickname() + " created the channel " + channel_name;
		Message msg(join_channel_msg);
		msg.set_receiver(channel_name);
		send_msg_queue.push(msg);

		return ;
	}

	// if channel exists add user to it
	channel->second.add_member(fd, nick);

	// send message to the channel that user joined
	std::string join_channel_msg = client_to_add->get_nickname() + " joined " + channel_name;
	Message msg(join_channel_msg);
	msg.set_receiver(channel_name);
	send_msg_queue.push(msg);

	return ;
}

void	Server::exec_cmd_KICK(Message &cmd_msg)
{
	// check if user is operator by iterating over operator list and check if msg fd is in that list
	// get fd of requesting user
	int const request_fd = cmd_msg.get_fd();

	// get channel the user should be kicked from
	std::string	const &channel_name = cmd_msg.get_arg(0);
	std::map<std::string, Channel>::iterator channel = channel_list.find(channel_name);

	// get the list of operators of that channel
	std::set<int> operator_list = channel->second.get_operator_list();
	std::set<int>::iterator iter;
	bool is_operator = false;
   
	// Loop over the operator_list
	for (iter = operator_list.begin(); iter != operator_list.end(); iter++)
	{
		if (request_fd == *iter)
		{
			is_operator = true;
		}
	}

	if (is_operator)
	{
		// find user to kick in member_list from msg_arg

		// if user to kick is not in member_list then throw error user not found

		// if user to kick is in member_list remove from member_list and create message that user was kicked
	}

	return ;
}

void	Server::exec_cmd_KNOCK(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_LINKS(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_LIST(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_LUSERS(Message &cmd_msg)
{
	push_msg(cmd_msg.get_fd(), ("251 " + cmd_msg.get_sender() + " :There are " +
		std::to_string(client_list.size()) + " users and 0 services on 1 server"));
	return ;
}

void	Server::exec_cmd_MAP(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_MODE(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_MOTD(Message &cmd_msg)
{
	int motd = 0;
	if (motd)
		(void)cmd_msg;
	else
		push_msg(cmd_msg.get_fd(), "422 " + cmd_msg.get_sender() + " :MOTD File is missing");
	return ;
}

void	Server::exec_cmd_NAMES(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

// registering the session
// !!!! need to handle changing the nickname
void	Server::exec_cmd_NICK(Message &cmd_msg)
{
	std::string	arg = cmd_msg.get_arg(0);
	int			fd = cmd_msg.get_fd();

	if (arg.empty())
	{
		push_msg(fd, "431 : No nickname entered");
		return ;
	}
	std::map<int, Client>::iterator	it;	

	// for matching nickname check
	for (it = client_list.begin(); it != client_list.end(); it++)
	{
		std::string	nick = it->second.get_nickname();
		if (nick == arg)
		{
			// if the nickname is equal to the arg, and the fd matches, change it
			if (it->first == fd)
			{
				// user is entering same nick as what they already have 
				it->second.set_nickname(arg);
				std::string full = ":" + nick + "! __ @ __ NICK " + it->second.get_nickname();
				Message msg(fd, full);
				// Channel chan()
				// send_channel_msg(msg, );
			}
			else
				push_msg(fd, ("433 " + nick + " " + arg + " :Nickname already taken"));
			return ;
		}
	}
	// to change after matching check
	for (it = client_list.begin(); it != client_list.end(); it++)
	{
		if (it->first == fd)
		{
			std::string temp = it->second.get_nickname();
			it->second.set_nickname(arg);
			std::string full = ":" + temp + "! __ @ __ NICK " + it->second.get_nickname();
			Message msg(fd, full);
		}
	}

}

void	Server::exec_cmd_NOTICE(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_PART(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_PASS(Message &cmd_msg)
{
	if (cmd_msg.get_arg(0) == "")
		reply_461(cmd_msg.get_fd(), "PASS", cmd_msg.get_sender());
	else if (cmd_msg.get_arg(0) == _password)
		
	return ;
}

void	Server::exec_cmd_PING(Message &cmd_msg)
{
	if (cmd_msg.get_arg(0) != "")
		push_msg(cmd_msg.get_fd(), ("PONG " + _hostname + " :" + cmd_msg.get_arg(0)));
	else
		push_msg(cmd_msg.get_fd(), ("409 " + cmd_msg.get_sender() + " :No origin specified"));
	return ;
}

void	Server::exec_cmd_PONG(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_PRIVMSG(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_QUIT(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_RULES(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_SETNAME(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_SILENCE(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_STATS(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_USER(Message &cmd_msg)
{
	std::string	arg = cmd_msg.get_arg(0);
	int			fd = cmd_msg.get_fd();
	if (arg.empty())
	{
		push_msg(fd, "431 : No username entered");
		return ;
	}
	std::map<int, Client>::iterator	it;

	// check if the param of user is already contained in the client list
	//		if yes - return the error
	for (it = client_list.begin(); it != client_list.end(); it++)
	{
		std::string	current = it->second.get_username();
		if (current == arg)
		{
			// if the nickname is equal to the arg, and the fd matches, change it
			if (it->first == fd)
			{
				it->second.set_username(arg);
				std::string full = ":" + current + "! __ @ __ NICK " + it->second.get_username() + "\r\n";
				Message msg(fd, full);
				// Channel chan()
				// send_channel_msg(msg, );

			}
			else
				push_msg(fd, ("433 " + current + " " + arg + " :Nickname already taken"));
			return ;
		}
	}
	// if not, create a new instance of the client class and add to the client list map
	// along with the fd of the socket 
	Client new_client(fd, arg);
	client_list.insert(std::make_pair(fd, new_client));
	cmd_msg.set_sender(new_client.get_username());
	// send a message back?
	push_msg(fd, ("001 " + new_client.get_username() + " :Hi, welcome to IRC"));
	push_msg(fd, ("002 " + new_client.get_username() + " :Your host is " +
		_hostname + ", running version ALISTIM-v0.01"));
	// could generate a timestamp when server class initialized to use here
	push_msg(fd, ("003 " + new_client.get_username() + " :This server was created 2022AD"));
	push_msg(fd, ("004 " + new_client.get_username() + " " + _hostname + "ALISTIM-v0.01 o o"));
	exec_cmd_LUSERS(cmd_msg);
	exec_cmd_MOTD(cmd_msg);
}

void	Server::exec_cmd_VERSION(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_WHO(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_WHOIS(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

void	Server::exec_cmd_WHOWAS(Message &cmd_msg)
{
	(void)cmd_msg;
	return ;
}

#include "Server.hpp"

/* BAN <#channel> <nickname> */
void	Server::exec_cmd_BAN(Message &cmd_msg)
{
	// check if user is operator by iterating over operator list and check if msg fd is in that list
	// get fd of requesting user
	int const request_fd = cmd_msg.get_fd();

 	// get channel the user should be banned from
	std::string	const channel_name = cmd_msg.get_arg(0);
	std::map<std::string, Channel>::iterator channel = channel_list.find(channel_name);

	if (channel == channel_list.end())
	{
		// no channel
		return;
	}

	if (is_operator(channel_name, request_fd))
	{
		// find user to ban in member_list from msg_arg
		std::string nick = cmd_msg.get_arg(1);
		int nick_fd = -1;
		std::map<int, std::string> member_list = channel->second.get_member_list();
		for (std::map<int, std::string>::iterator it = member_list.begin(); it != member_list.end(); it++)
		{
			if (it->second == nick)
				nick_fd = it->first;
		}

		// if user to ban is not in member_list then throw error user not found
		if (nick_fd == -1)
		{
			// throw error that user not found
			return;
		}

		// if user to ban is in member_list remove from member_list
		channel->second.remove_member(nick_fd);

		// add user to ban list 
		channel->second.ban_user(nick);

		// create message that user was banned
		push_msg(request_fd, nick + " was banned from channel: " + channel_name);
		push_msg(nick_fd, nick + " You were banned from channel: " + channel_name);
	}

	return ;
}

void	Server::exec_cmd_INVITE(Message &cmd_msg)
{
	// get fd of requesting user
	int const request_fd = cmd_msg.get_fd();
	std::string nick = cmd_msg.get_arg(0);
	std::string	const &channel_name = cmd_msg.get_arg(1);

	// Check if channel_name has correct syntax
	if (!(channel_name[0] == '#' || channel_name[0] == '&'))
	{
		// throw error
		return ;
	}

	std::map<std::string, Channel>::iterator channel = channel_list.find(channel_name);
	if (channel == channel_list.end())
	{
		// Create channel
		// Add nick to channel
	}
	else
	{
		// check if sender is member of the channel
		std::map<int, std::string> member_list = channel->second.get_member_list();
		if (member_list.find(request_fd) == member_list.end())
		{
			// throw error - requesting user is not member of the channel.
			return;
		}

		// check if the channel mode is invite only
			// if yes check if sender is an operator
		// Add nick to channel
	}

	return ;
}

void	Server::exec_cmd_JOIN(Message &cmd_msg)
{
	// get fd from user joining channel
	int	const fd = cmd_msg.get_fd();

	// get the channel name from the argument
	std::string	const &channel_name = cmd_msg.get_arg(0);

	// Check if channel_name has correct syntax
	if (!(channel_name[0] == '#' || channel_name[0] == '&'))
	{
		// throw error
		return ;
	}

	// get client who wants to join channel by fd
	Client *client_to_add = get_client(fd);
	if (client_to_add == NULL)
		return ;

	// get nickname of client
	std::string nick = client_to_add->get_nickname();
	std::string username = client_to_add->get_username();
	std::string host = get_host(fd);

	std::map<std::string, Channel>::iterator channel = channel_list.find(channel_name);

	bool is_banned = false;

	// if channel doesn't exist create one with caller as operator/channel_owner
	if (channel == channel_list.end())
	{
		channel_list.insert(std::make_pair(channel_name, Channel(channel_name, fd)));
		channel = channel_list.find(channel_name);
		channel->second.add_operator(fd);
		channel->second.add_member(fd, nick);
		client_to_add->add_to_channel_list(channel_name);

		// Create initial channel message
		std::string join_msg = ":" + nick + "!" + username + "@" + host + " JOIN :" + channel_name;
		push_msg(fd, join_msg);

		std::string msg_331 = "331 " + nick + " " + channel_name + " :No topic is set";
		push_msg(fd, msg_331);

		std::string msg_353 = "353 " + nick + " = " + channel_name + " :" + nick;
		push_msg(fd, msg_353);

		// need to change later to hold all clients (nicks) inside of channel 
		std::string msg_366 = "366 " + nick + " " + channel_name + " :End of NAMES list";
		push_msg(fd, msg_366);

		// msg.set_receiver(channel_name);
		// send_msg_queue.push(chan_msg);
		// push_msg()
		return ;
	}

	std::set<std::string> ban_list = channel->second.get_ban_list();
	std::set<std::string>::iterator iter;

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

	// if channel exists add user to it
	channel->second.add_member(fd, nick);
	client_to_add->add_to_channel_list(channel_name);

	std::string join_msg = ":" + nick + "!" + username + "@" + host + " JOIN :" + channel_name;
	push_msg(fd, join_msg);

	// send message to the channel that user joined
	std::string msg_331 = "331 " + nick + " " + channel_name + " :No topic is set";
	push_msg(fd, msg_331);

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

void	Server::exec_cmd_LUSERS(Message &cmd_msg)
{
	push_msg(cmd_msg.get_fd(), ("251 " + cmd_msg.get_sender() + " :There are " +
		std::to_string(client_list.size()) + " users and 0 services on 1 server"));
	return ;
}

void	Server::exec_cmd_MODE(Message &cmd_msg)
{
	// OBSOLETE
	// (void)cmd_msg;
	std::cout << "MODE msg: " << cmd_msg.get_arg(0) << std::endl;
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
				std::string full = ":" + nick + "!" + it->second.get_username();
					+ "@" + host_ips.find(fd)->second
					+ " NICK " + it->second.get_nickname();
				Message msg(fd, full);
				// msg.set_receiver()
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
			std::string old_nick = it->second.get_nickname();
			it->second.set_nickname(arg);
			std::string full = ":" + old_nick + "!" + it->second.get_username();
				+ "@" + host_ips.find(fd)->second
				+ " NICK " + it->second.get_nickname();
			Message msg(fd, full);

			// find tne channels that the user is in (if any)
			if (it->second.get_channel_list().empty() == true)
			{
				push_msg(fd, full);
			}
			else
				std::cout << "Channel list:" << it->second.get_channel_list().at(0) << std::endl;
				// add these to the reciever part of message?
		}
	}

}

void	Server::exec_cmd_NOTICE(Message &cmd_msg)
{
	std::string	arg = cmd_msg.get_arg(0);
	std::string msg_from_arg = cmd_msg.get_arg(1);
	bool		is_channel_msg = false;

	if (arg[0] == '#' || arg[0] == '&')
		is_channel_msg = true;

	if (is_channel_msg)
	{
		std::string	const &channel_name = cmd_msg.get_arg(0);
		std::map<std::string, Channel>::iterator channel = channel_list.find(channel_name);
		if (channel == channel_list.end())
		{
			// throw error that channel doesn't exist
			return ;
		}
		push_multi_msg(channel->second, msg_from_arg, -1);
	}
	else
	{
		std::map<int, Client>::iterator	it;	
		// search for user matching the nickname
		for (it = client_list.begin(); it != client_list.end(); it++)
		{
			std::string	nick = it->second.get_nickname();
			if (nick == arg)
			{
				int receiver_fd = it->second.get_fd();
				// Push message to the queue
				// push_msg(it->first, ("433 " + nick + " " + arg + " :Nickname already taken"));
				push_msg(receiver_fd, msg_from_arg);
				return ;
			}
		}
	}
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
	std::string	arg = cmd_msg.get_arg(0);
	// std::string msg_from_arg = cmd_msg.get_arg(1);
	std::string msg_from_arg;
	bool		is_channel_msg = false;

	if (arg[0] == '#' || arg[0] == '&')
		is_channel_msg = true;

	// go through every arg after 1 until empty is found
	std::vector<std::string> arg_vector = cmd_msg.get_arg_vector();
	for (std::vector<std::string>::const_iterator i = arg_vector.begin() + 1; i != arg_vector.end(); i++)
		msg_from_arg += *i;

	if (is_channel_msg)
	{
		std::string	const &channel_name = cmd_msg.get_arg(0);
		std::map<std::string, Channel>::iterator channel = channel_list.find(channel_name);
		if (channel == channel_list.end())
		{
			// throw error that channel doesn't exist
			return ;
		}
		Client *sender_client = get_client(cmd_msg.get_fd());
		std::string host = get_host(cmd_msg.get_fd());
		push_multi_msg(channel->second, ":" + sender_client->get_nickname() + "!" + sender_client->get_username() + "@" + host + " PRIVMSG " + channel_name + " :" + msg_from_arg, cmd_msg.get_fd());
	}
	else
	{
		std::cout << "Goes in here" << std::endl;
		std::map<int, Client>::iterator	it;	
		// search for user matching the nickname
		for (it = client_list.begin(); it != client_list.end(); it++)
		{
			std::string	nick = it->second.get_nickname();
			if (nick == arg)
			{
				std::cout << "Goes in here" << std::endl;
				int receiver_fd = it->second.get_fd();
				// Push message to the queue
				// push_msg(it->first, ("433 " + nick + " " + arg + " :Nickname already taken"));
				Client *sender_client = get_client(cmd_msg.get_fd());
				std::string host = get_host(cmd_msg.get_fd());
				push_msg(receiver_fd, ":" + sender_client->get_nickname() + "!" + sender_client->get_username() + "@" + host + " PRIVMSG " + nick + " :" + msg_from_arg);
				return ;
			}
		}
	}
	return ;
}

void	Server::exec_cmd_TOPIC(Message &cmd_msg)
{
	std::string	const &channel_name = cmd_msg.get_arg(0);
	std::string const &channel_topic = cmd_msg.get_arg(1);

	std::map<std::string, Channel>::iterator channel = channel_list.find(channel_name);
	if (channel == channel_list.end())
	{
		// throw error that channel doesn't exist
		return ;
	}
	channel->second.set_channel_topic(channel_topic);
}

void	Server::exec_cmd_USER(Message &cmd_msg)
{
	(void)cmd_msg;
	//unsupported command!
}

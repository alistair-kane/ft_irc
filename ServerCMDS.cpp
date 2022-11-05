#include "Server.hpp"

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
		std::string join_channel_msg = client_to_add->get_nickname() + " created the channel " + channel_name;
		Message msg(join_channel_msg);
		msg.set_receiver(channel_name);
		send_msg_queue.push(msg);
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

void	Server::exec_cmd_LUSERS(Message &cmd_msg)
{
	push_msg(cmd_msg.get_fd(), ("251 " + cmd_msg.get_sender() + " :There are " +
		std::to_string(client_list.size()) + " users and 0 services on 1 server"));
	return ;
}

void	Server::exec_cmd_MODE(Message &cmd_msg)
{
	// OBSOLETE
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
	// TODO
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
	std::string	arg = cmd_msg.get_arg(0);
	std::string msg_from_arg = cmd_msg.get_arg(1);
	int			fd = cmd_msg.get_fd();
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
		push_msg(fd, msg_from_arg);
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
				push_msg(fd, msg_from_arg);
				return ;
			}
		}
	}
	return ;
}

void	Server::exec_cmd_USER(Message &cmd_msg)
{
	(void)cmd_msg;
	//unsupported command!
}

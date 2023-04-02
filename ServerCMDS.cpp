#include "Server.hpp"

/* INVITE <nick> <channel>*/
void	Server::exec_cmd_INVITE(Message &cmd_msg)
{
	// get fd of requesting user
	int const request_fd = cmd_msg.get_fd();
	std::string nick = cmd_msg.get_arg(0);
	std::string	const &channel_name = cmd_msg.get_arg(1);

	std::map<std::string, Channel>::iterator channel = channel_list.find(channel_name);
	if (channel == channel_list.end()) // channel doesn't exist
		return;

	if (!(channel->second.is_operator(request_fd))) // requesting user is not an operator of the channel.
		return;

	Client *to_invite = get_client_by_nick(nick);
	if (to_invite == NULL) // nick doesn't exist
		return;

	channel->second.invite_user(nick);
	Client *requester = get_client(request_fd);
	push_msg(request_fd, ":" + format_nick_user_host(requester) + " 341 " + requester->get_nickname() + " " + nick + " " + channel_name);
	push_msg(to_invite->get_fd(), ":" + format_nick_user_host(get_client(request_fd)) + " " + cmd_msg.get_cmd() + " " + nick + " " + channel_name);
}

void	Server::exec_cmd_JOIN(Message &cmd_msg)
{
	// get fd from user joining channel
	int	const fd = cmd_msg.get_fd();

	// get the channel name from the argument
	std::string	const &channel_name = cmd_msg.get_arg(0);
	std::string key = cmd_msg.get_arg(1);

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

	if (channel != channel_list.end() && !(channel->second.can_client_join(client_to_add, key)))
			return;

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

		push_msg(fd, ":" + format_nick_user_host(get_client(fd)) + " MODE " + channel_name + " +o " + nick);


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

/* KICK <channel> <user> [<comment>] */
void	Server::exec_cmd_KICK(Message &cmd_msg)
{
	// check if user is operator by iterating over operator list and check if msg fd is in that list
	// get fd of requesting user
	int const request_fd = cmd_msg.get_fd();

	// get channel the user should be kicked from
	std::string	const &channel_name = cmd_msg.get_arg(0);
	std::map<std::string, Channel>::iterator channel = channel_list.find(channel_name);

	if (channel == channel_list.end()) // channel doesn't exist
		return;

	if (!(channel->second.is_operator(request_fd))) // requesting user is not an operator of the channel.
		return;

	std::string nick = cmd_msg.get_arg(1);
	Client *target = this->get_client_by_nick(nick);
	if (target == NULL)
		return;

	if (channel->second.is_member(target->get_fd()))
	{
		std::string msg = cmd_msg.get_arg(2);
		if (msg.length() != 0)
			msg = " :" + msg;
		channel->second.remove_member(target->get_fd());
		push_msg(request_fd, ":" + format_nick_user_host(get_client(request_fd)) + " KICK " + cmd_msg.get_arg(0) + " " + nick + msg);
		push_msg(target->get_fd(), ":" + format_nick_user_host(get_client(request_fd)) + " KICK " + cmd_msg.get_arg(0) + " " + nick + msg);
	}	
	return ;
}

void	Server::exec_cmd_LUSERS(Message &cmd_msg)
{
	push_msg(cmd_msg.get_fd(), ("251 " + cmd_msg.get_sender() + " :There are " +
		std::to_string(client_list.size()) + " users and 0 services on 1 server"));
	return ;
}

/* MODE <channel> {[+|-]|o|p|s|i|t|n|l|b|v|k} [<limit>] [<user>] [<ban mask>] */
void Server::exec_cmd_CHANNEL_MODE(Message &cmd_msg, Channel *channel)
{
	std::string flags = cmd_msg.get_arg(1);
	const int op_fd = cmd_msg.get_fd();
	if (flags.find('-') != std::string::npos && flags.find('+') != std::string::npos)
		return; // Wrong format

	if (flags.find('o') != std::string::npos) // give/take channel operator priviligies
	{
		std::string nick = cmd_msg.get_arg(2);
		int client_fd = get_client_fd(nick);
		if (channel->is_member(client_fd))
		{
			if (flags.find('-') != std::string::npos)
				channel->remove_operator(client_fd);
			else
				channel->add_operator(client_fd);
			push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags + " " + nick);
			push_msg(client_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags + " " + nick);
		}
	}

	if (flags.find('p') != std::string::npos) // private channel flag
	{
		if (flags.find('-') != std::string::npos)
			channel->set_channel_private(0);
		else
			channel->set_channel_private(1);
		push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags);
	}

	if (flags.find('s') != std::string::npos) // secret channel flag
	{
		if (flags.find('-') != std::string::npos)
			channel->set_channel_secret(0);
		else
			channel->set_channel_secret(1);
		push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags);
	}

	if (flags.find('i') != std::string::npos) // invite-only channel flag
	{
		if (flags.find('-') != std::string::npos)
			channel->set_channel_inviteonly(0);
		else
			channel->set_channel_inviteonly(1);
		push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags);
	}

	if (flags.find('t') != std::string::npos) // t - topic settable by channel operator only flag;
	{
		if (flags.find('-') != std::string::npos)
			channel->set_channel_topic_settable(0);
		else
			channel->set_channel_topic_settable(1);
		push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags);
	}

	if (flags.find('n') != std::string::npos) // n - no messages to channel from clients on the outside;
	{
		if (flags.find('-') != std::string::npos)
			channel->set_channel_inside_only(0);
		else
			channel->set_channel_inside_only(1);
		push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags);
	}

	if (flags.find('m') != std::string::npos) // m - moderated channel;
	{
		if (flags.find('-') != std::string::npos)
			channel->set_channel_moderated(0);
		else
			channel->set_channel_moderated(1);
		push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags);
	}

	if (flags.find('l') != std::string::npos) // l - set the user limit to channel;
	{
		if (flags.find('-') != std::string::npos)
		{
			channel->set_channel_limited(0);
			push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags);
		}
		else
		{
			std::string s_limit = cmd_msg.get_arg(2);
			if (s_limit.length() > 0)
			{
				int limit = std::stoi(s_limit);
				if (limit > 0)
				{
					channel->set_channel_limited(1);
					channel->set_limit(limit);
					push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags + " :" + std::to_string(limit));
				}
			}
		}
	}

	if (flags.find('b') != std::string::npos) // b - set a ban mask to keep users out;
	{
		std::string nick = cmd_msg.get_arg(2);
		int client_fd = get_client_fd(nick);
		if (flags.find('-') != std::string::npos)
		{
			channel->unban_user(nick);
		}
		else
		{
			channel->ban_user(nick);
			if (channel->is_member(client_fd))
			{
				channel->remove_member(client_fd);
				push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " KICK " + cmd_msg.get_arg(0) + " " + nick);
				push_msg(client_fd, ":" + format_nick_user_host(get_client(op_fd)) + " KICK " + cmd_msg.get_arg(0) + " " + nick);
			}
		}
		push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags + " " + nick);
		push_msg(client_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags + " " + nick);
	}

	if (flags.find('v') != std::string::npos) // v - give/take the ability to speak on a moderated channel;
	{
		if (channel->is_channel_moderated())
		{
			std::string nick = cmd_msg.get_arg(2);
			int client_fd = get_client_fd(nick);
			if (flags.find('-') != std::string::npos)
				channel->cant_talk_user(nick);
			else
				channel->can_talk_user(nick);
			push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags + " " + nick);
			push_msg(client_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags + " " + nick);
		}
	}

	if (flags.find('k') != std::string::npos) // k - set a channel key (password)
	{
		if (flags.find('-') != std::string::npos)
			channel->set_key("");
		else
			channel->set_key(cmd_msg.get_arg(2));
		push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags);
	}
}

/* MODE <nickname> {[+|-]|i|w|s|o}*/
void Server::exec_cmd_USER_MODE(Message &cmd_msg, Client *client)
{
	std::string flags = cmd_msg.get_arg(1);
	const int op_fd = cmd_msg.get_fd();
	std::string nick = cmd_msg.get_arg(2);
	if (flags.find('-') != std::string::npos && flags.find('+') != std::string::npos)
		return; // Wrong format

	if (flags.find('o') != std::string::npos) // give/take operator priviligies
	{
		if (flags.find('-') != std::string::npos)
			client->set_operator(0);
		else
			client->set_operator(1);
		push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags + " " + nick);
		push_msg(client->get_fd(), ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags + " " + nick);
	}

	if (flags.find('i') != std::string::npos) // become invisible
	{
		if (flags.find('-') != std::string::npos)
			client->set_invisible(0);
		else
			client->set_invisible(1);
		push_msg(op_fd, ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags + " " + nick);
		push_msg(client->get_fd(), ":" + format_nick_user_host(get_client(op_fd)) + " " + cmd_msg.get_cmd() + " " + cmd_msg.get_arg(0) + " " + flags + " " + nick);
	}
}

void	Server::exec_cmd_MODE(Message &cmd_msg)
{
	std::string	const &target_name = cmd_msg.get_arg(0); // channel or nickname
	const int fd = cmd_msg.get_fd();
	// Setting the mode of a channel
	if (target_name[0] == '#' || target_name[0] == '&')
	{
		std::map<std::string, Channel>::iterator channel = channel_list.find(target_name);
		Client *client = get_client(fd);
		if (channel == channel_list.end())
		{
			//throw error: No channel
			return;
		}
		if (!(is_operator(target_name, fd) || client->is_operator()))
		{
			//throw error: Not operator
			return;
		}
		exec_cmd_CHANNEL_MODE(cmd_msg, &(channel->second));
	}
	// Setting the mode of a user
	else
	{
		Client *client = get_client_by_nick(target_name);
		if (client == NULL)
		{
			//throw error: No client
			return;
		}
		if (!client->is_operator())
		{
			//throw error: Not operator
			return;
		}
		exec_cmd_USER_MODE(cmd_msg, client);
	}
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
	std::string	target_name = cmd_msg.get_arg(0);
	// std::string msg_from_arg = cmd_msg.get_arg(1);
	std::string msg_from_arg;
	bool		is_channel_msg = false;

	if (target_name[0] == '#' || target_name[0] == '&')
		is_channel_msg = true;

	// go through every arg after 1 until empty is found
	std::vector<std::string> arg_vector = cmd_msg.get_arg_vector();
	for (std::vector<std::string>::const_iterator i = arg_vector.begin() + 1; i != arg_vector.end(); i++)
		msg_from_arg += *i;

	if (is_channel_msg)
	{
		std::map<std::string, Channel>::iterator channel = channel_list.find(target_name);
		if (channel == channel_list.end())
		{
			// throw error that channel doesn't exist
			return ;
		}
		Client *sender_client = get_client(cmd_msg.get_fd());
		if (sender_client != NULL && channel->second.can_client_talk(sender_client))
			push_multi_msg(channel->second, format_msg(sender_client, "NOTICE", target_name, msg_from_arg), cmd_msg.get_fd());
	}
	else
	{
		int receiver_fd = this->get_client_fd(target_name);
		
		// Push message to the queue
		// push_msg(it->first, ("433 " + nick + " " + arg + " :Nickname already taken"));
		Client *sender_client = get_client(cmd_msg.get_fd());
		push_msg(receiver_fd, format_msg(sender_client, "NOTICE", target_name, msg_from_arg));
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
	std::string	target_name = cmd_msg.get_arg(0);
	// std::string msg_from_arg = cmd_msg.get_arg(1);
	std::string msg_from_arg;
	bool		is_channel_msg = false;

	if (target_name[0] == '#' || target_name[0] == '&')
		is_channel_msg = true;

	// go through every arg after 1 until empty is found
	std::vector<std::string> arg_vector = cmd_msg.get_arg_vector();
	for (std::vector<std::string>::const_iterator i = arg_vector.begin() + 1; i != arg_vector.end(); i++)
		msg_from_arg += *i;

	if (is_channel_msg)
	{
		std::map<std::string, Channel>::iterator channel = channel_list.find(target_name);
		if (channel == channel_list.end())
		{
			// throw error that channel doesn't exist
			return ;
		}
		Client *sender_client = get_client(cmd_msg.get_fd());
		if (sender_client != NULL && channel->second.can_client_talk(sender_client))
			push_multi_msg(channel->second, format_msg(sender_client, "PRIVMSG", target_name, msg_from_arg), cmd_msg.get_fd());
	}
	else
	{
		int receiver_fd = this->get_client_fd(target_name);
		
		// Push message to the queue
		// push_msg(it->first, ("433 " + nick + " " + arg + " :Nickname already taken"));
		Client *sender_client = get_client(cmd_msg.get_fd());
		push_msg(receiver_fd, format_msg(sender_client, "PRIVMSG", target_name, msg_from_arg));
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

	if (channel->second.is_channel_topic_settable() && !(channel->second.is_operator(cmd_msg.get_fd())))
	{
		// this user can't set topic on channel
		return;
	}
	channel->second.set_channel_topic(channel_topic);
}

void	Server::exec_cmd_USER(Message &cmd_msg)
{
	(void)cmd_msg;
	//unsupported command!
}

void	Server::exec_cmd_QUIT(Message &cmd_msg)
{
	int	const & fd = cmd_msg.get_fd();
	for (std::map<std::string, Channel>::iterator channel_it = this->channel_list.begin(); channel_it != this->channel_list.end(); channel_it++)
	{
		if (channel_it->second.is_member(fd))
		{
			channel_it->second.remove_member(fd);
			push_multi_msg(channel_it->second, ":" + format_nick_user_host(get_client(fd)) + " " + cmd_msg.get_cmd(), cmd_msg.get_fd());
		}
	}
	this->client_list.erase(fd);
	this->host_ips.erase(fd);
	this->reg_clients.erase(std::find(this->reg_clients.begin(), this->reg_clients.end(), fd));
	close(fd);
}

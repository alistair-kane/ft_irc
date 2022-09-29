#include "Server.hpp"

int	Server::exec_cmd_ADMIN(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_AWAY(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_INVITE(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_JOIN(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_KICK(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_KNOCK(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_LINKS(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_LIST(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_LUSERS(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_MAP(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_MODE(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_MOTD(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_NAMES(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

// registering the session
// !!!! need to handle changing the nickname
int	Server::exec_cmd_NICK(Message &cmd_msg)
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
			return 1;
		}
	}

	// if not, create a new instance of the client class and add to the client list map
	// along with the fd of the socket 
	Client new_client(cmd_msg.get_fd(), cmd_msg.get_arg());
	client_list.insert({cmd_msg.get_fd(), new_client});
	// send a message back?
	return (0);
}

int	Server::exec_cmd_NICK(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_NOTICE(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_PART(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_PASS(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_PING(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_PONG(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_PRIVMSG(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_QUIT(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_RULES(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_SETNAME(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_SILENCE(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_STATS(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_USER(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_VERSION(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_WHO(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_WHOIS(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

int	Server::exec_cmd_WHOWAS(Message &cmd_msg)
{
	(void)cmd_msg;
	return (0);
}

#include "Channel.hpp"

void Channel::add_member(int const &fd, std::string const &nick)
{
	member_list.insert(std::make_pair(fd, nick));
}

void	Channel::remove_member(int const &fd)
{
	member_list.erase(fd);
}

void	Channel::ban_user(std::string const & nick)
{
	ban_list.insert(nick);
}

void	Channel::unban_user(std::string const & nick)
{
	ban_list.erase(nick);
}

std::map<int, std::string> const &Channel::get_member_list(void) const
{
	return (this->member_list);
}

std::set<std::string> const	&Channel::get_ban_list(void) const
{
	return (this->ban_list);
}

void	Channel::set_channel_topic(std::string const & input)
{
	this->channel_topic = input;
}

std::string const &	Channel::get_channel_topic(void) const
{
	return (this->channel_topic);
}

void	Channel::set_channel_private(bool const & invite)
{
	this->is_private_channel = invite;
}

bool	Channel::is_channel_private(void) const
{
	return (this->is_private_channel);
}

void	invite_user(std::string const & nick)
{
	
}
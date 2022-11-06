#include "Channel.hpp"

/* Constructors and Destructor */
Channel::Channel(std::string const &channel_name, int const & fd) :channel_name(channel_name)
{
	this->operator_list.insert(fd);
	this->is_private_channel = false;
	this->ban_list.insert(std::string());
}

Channel::~Channel(void)
{}

/* Ban operations */
void	Channel::ban_user(std::string const & nick)
{
	ban_list.insert(nick);
}

void	Channel::unban_user(std::string const & nick)
{
	ban_list.erase(nick);
}

/* Invite operations */
// void	invite_user(std::string const & nick)
// {
	
// }

void	Channel::set_channel_private(bool const & invite)
{
	this->is_private_channel = invite;
}

bool	Channel::is_channel_private(void) const
{
	return (this->is_private_channel);
}

/* Member operations */
void Channel::add_member(int const &fd, std::string const &nick)
{
	member_list.insert(std::make_pair(fd, nick));
}

void	Channel::remove_member(int const &fd)
{
	member_list.erase(fd);
}

/* Operator operations */
void	Channel::add_operator(int const &fd)
{
	operator_list.insert(fd);
}

void	Channel::remove_operator(int const &fd)
{
	operator_list.erase(fd);
}

/* Getters and Setters */
std::map<int, std::string> const &Channel::get_member_list(void) const
{
	return (this->member_list);
}

std::set<int> const &Channel::get_operator_list(void) const
{
	return (this->operator_list);
}
std::set<std::string> const	&Channel::get_ban_list(void) const
{
	return (this->ban_list);
}

std::string const &	Channel::get_channel_topic(void) const
{
	return (this->channel_topic);
}

void	Channel::set_channel_topic(std::string const & input)
{
	this->channel_topic = input;
}

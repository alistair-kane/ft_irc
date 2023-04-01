#include "Channel.hpp"

/* Constructors and Destructor */
Channel::Channel(std::string const &channel_name, int const & fd) :channel_name(channel_name)
{
	this->operator_list.insert(fd);
	this->is_private_channel = false;
	this->is_secret_channel = false;
	this->is_inviteonly_channel = false;
	this->is_topic_settable = false; // by operator only
	this->is_inside_only_chanel = true;
	this->is_moderated_channel = false;
	this->is_limited_channel = false;
	this->ban_list.insert(std::string());
	this->can_talk_list.insert(std::string());
	this->invite_list.insert(std::string());
	this->user_limit = -1;
	this->key = "";
}

Channel::~Channel(void)
{}

/* Restriction lists operations */
void	Channel::ban_user(std::string const & nick)
{
	ban_list.insert(nick);
}

void	Channel::unban_user(std::string const & nick)
{
	ban_list.erase(nick);
}

void	Channel::can_talk_user(std::string const & nick)
{
	can_talk_list.insert(nick);
}

void	Channel::cant_talk_user(std::string const & nick)
{
	can_talk_list.erase(nick);
}

void	Channel::invite_user(std::string const & nick)
{
	invite_list.insert(nick);
}

/* Privacy operations */
void	Channel::set_channel_private(bool const & yes)
{
	this->is_private_channel = yes;
}

bool	Channel::is_channel_private(void) const
{
	return (this->is_private_channel);
}

void	Channel::set_channel_secret(bool const & yes)
{
	this->is_secret_channel = yes;
}

bool	Channel::is_channel_secret(void) const
{
	return (this->is_secret_channel);
}

void	Channel::set_channel_inviteonly(bool const & yes)
{
	this->is_secret_channel = yes;
}

bool	Channel::is_channel_inviteonly(void) const
{
	return (this->is_secret_channel);
}

bool	Channel::is_channel_topic_settable(void) const
{
	return (this->is_topic_settable);
}

void	Channel::set_channel_topic_settable(bool const &yes)
{
	this->is_topic_settable = yes;
}

bool	Channel::is_channel_inside_only(void) const
{
	return (this->is_inside_only_chanel);
}

void	Channel::set_channel_inside_only(bool const &yes)
{
	this->is_inside_only_chanel = yes;
}

bool	Channel::is_channel_moderated(void) const
{
	return (this->is_moderated_channel);
}

void	Channel::set_channel_moderated(bool const &yes)
{
	this->is_moderated_channel = yes;
}

bool	Channel::is_channel_limited(void) const
{
	return (this->is_limited_channel);
}

void	Channel::set_channel_limited(bool const &yes)
{
	this->is_limited_channel = yes;
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

bool	Channel::is_member(int const &fd)
{
	return (this->member_list.find(fd) != this->member_list.end());
}

bool	Channel::is_operator(int const &fd)
{
	return (this->operator_list.find(fd) != this->operator_list.end());
}

void	Channel::set_limit(int l)
{
	this->user_limit = l;
}

void	Channel::set_key(std::string k)
{
	this->key = k;
}

bool	Channel::can_client_join(Client *client_to_add, std::string _key)
{
	bool can_join = 1;
	if (this->is_channel_secret() || this->is_channel_private() || this->is_channel_inviteonly())
	{
		if (this->invite_list.find(client_to_add->get_nickname()) == this->invite_list.end())
			can_join = 0;
	}
	if (this->ban_list.find(client_to_add->get_nickname()) != this->ban_list.end())
		can_join = 0;

	if (_key != this->key)
		can_join = 0;

	if (is_channel_limited() && user_limit <= (int) this->member_list.size())
		can_join = 0;

	return (can_join);
}

bool	Channel::can_client_talk(Client *client_to_talk)
{
	if (this->ban_list.find(client_to_talk->get_nickname()) != this->ban_list.end())
		return (0);
	if (this->is_channel_inside_only() && !(this->is_member(client_to_talk->get_fd())))
		return (0);
	if (this->is_channel_moderated() && (this->can_talk_list.find(client_to_talk->get_nickname()) == this->can_talk_list.end()))
		return (0);

	return (1);
}

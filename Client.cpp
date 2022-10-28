#include "Client.hpp"

// Client::Client(void) :fd(-1), nickname(""), username("")
// {}

Client::Client(int const & fd) :fd(fd), nickname(""), username("") //, auth(false)
{}

Client::Client(int const & fd, std::string const & nickname) :fd(fd), nickname(nickname), username("") //, auth(false)
{}

Client::Client(std::string const & nickname) :fd(-1), nickname(nickname), username(""), message_buffer("") //, auth(false)
{}

Client::Client(Client const & other) :fd(other.fd), nickname(other.nickname), username(other.username), message_buffer("") //, auth(false)
{}

Client & Client::operator=(Client const &rhs)
{
	this->~Client();
	new (this) Client(rhs);
	return (*this);
}

Client::~Client(void)
{}

// bool	Client::get_auth(void) const
// {
// 	return (this->auth);
// }

// void	Client::set_auth(void)
// {
// 	auth = true;
// }

std::string const &Client::get_nickname(void) const
{
	return (this->nickname);
}

void	Client::set_nickname(std::string const &nickname)
{
	this->nickname = nickname;
}

std::string const &Client::get_username(void) const
{
	return (this->username);
}

void	Client::set_username(std::string const &username)
{
	this->username = username;
}

bool Client::operator<(Client const &rhs) const
{
	return (this->nickname < rhs.nickname);
}


void	Client::clear_message_buffer(void)
{
	this->message_buffer.clear();
}

void	Client::append_message_buffer(char const *input)
{
	this->message_buffer.append(input);
}

void	Client::append_message_buffer(std::string const &input)
{
	this->message_buffer.append(input);
}

std::string const & Client::get_message_buffer(void) const
{
	return (this->message_buffer);
}

std::vector<std::string>	&Client::get_channel_list(void) const
{
	return (this->channel_list);
}

void Client::add_to_channel_list(std::string const &channel_name)
{
	this->channel_list.push_back(channel_name);
}

void	Client::remove_from_channel_list(std::string const &channel_name)
{
	this->channel_list.remove(channel_list.begin(), channel_list.end(), channel_name);
}
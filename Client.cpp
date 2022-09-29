#include "Client.hpp"

// Client::Client(void) :fd(-1), nickname(""), username("")
// {}

Client::Client(int const & fd) :fd(fd), nickname(""), username("")
{}

Client::Client(int const & fd, std::string const & nickname) :fd(fd), nickname(""), username("")
{}

Client::Client(std::string const & nickname) :fd(-1), nickname(nickname), username(""), message_buffer("")
{}

Client::Client(Client const & other) :fd(other.fd), nickname(other.nickname), username(other.username), message_buffer("")
{}

Client & Client::operator=(Client const & rhs)
{
	this->~Client();
	new (this) Client(rhs);
	return (*this);
}

Client::~Client(void)
{}

void	Client::set_nickname(std::string const & nickname)
{
	this->nickname = nickname;
}

bool Client::operator<(Client const & rhs) const
{
	return (this->nickname < rhs.nickname);
}

std::string const & Client::get_nickname(void) const
{
	return (this->nickname);
}

void	Client::clear_message_buffer(void)
{
	this->message_buffer.clear();
}

void	Client::append_message_buffer(char const * input)
{
	this->message_buffer.append(input);
}

void	Client::append_message_buffer(std::string const & input)
{
	this->message_buffer.append(input);
}

std::string const & Client::get_message_buffer(void) const
{
	return (this->message_buffer);
}
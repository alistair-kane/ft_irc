#include "Message.hpp"

Message::Message(int const & fd) : _fd(fd)
{}

Message::Message(int const & fd, std::string input) : _fd(fd), _raw(input)
{
	this->_legit = false;
	// if (this->_raw.find("\r\n") != std::string::npos)
		// this->_legit = true;
}

Message::Message(Message const & other)
{
	this->CMD = other.CMD;
	this->sender = other.sender;
	this->receiver = other.receiver;

	this->_raw = other._raw;
	this->_fd = other._fd;
	this->_legit = other._legit;
}

Message const & Message::operator=(Message const & rhs)
{
	this->CMD = rhs.CMD;
	this->_arg = rhs._arg;
	this->_raw = rhs._raw;
	return (*this);
}

Message::~Message(void)
{}

std::string const & Message::get_cmd(void) const
{
	return (this->CMD);
}

std::string const & Message::get_arg(void) const
{
	return (this->_arg);
}

std::string const & Message::get_sender(void) const
{
	return (this->sender);
}

// std::string const & Message::print_message(void) const
// {
// 	std::cout	<< "fd: [" << _fd << "]\n"
// 				<< "prefix: [" << this->prefix << "]\n"
// 				<< "CMD: [" << this->CMD << "]\n"
// 				<< "arg: [" << this->arg << "]\n"
// 				<< "postfix: [" << this->postfix << "]\n";
// 	//return (this->CMD + " " + this->sender + " " + this->receiver + " " + this->content + "\r\n")
// 	return (_raw);
// }

int const & Message::get_fd(void) const
{
	return (_fd);
}

void	Message::set_cmd(std::string cmd)
{
	this->CMD = cmd;
}

void	Message::set_arg(std::string arg)
{
	this->_arg = arg;
}

void	Message::set_sender(std::string sender)
{
	this->sender = sender;
}

size_t Message::msg_len(void) const
{
	return ((_raw.length()));
}

const char *Message::raw_msg(void) const
{
	return (_raw.c_str());
}

std::string const & Message::get_receiver(void) const
{
	return (this->receiver);
}

void	set_receiver(std::string receiver)
{
	receiver = receiver;
}

bool	Message::receiver_is_channel(void) const
{
	if (receiver[0] == '#' || receiver[0] == '&')
		return (true);
	return (false);
}
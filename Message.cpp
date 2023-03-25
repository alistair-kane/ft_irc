#include "Message.hpp"

Message::Message(int const & fd) : _fd(fd)
{}

Message::Message(int const & fd, std::string input) : _fd(fd), _raw(input)
{
	this->_legit = false;
	// if (this->_raw.find("\r\n") != std::string::npos)
		// this->_legit = true;
}

Message::Message(std::string input) : _raw(input)
{}

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
	this->_args = rhs._args;
	this->_raw = rhs._raw;
	return (*this);
}

Message::~Message(void)
{}

std::string const & Message::get_cmd(void) const
{
	return (this->CMD);
}

std::string Message::get_arg(int idx) const
{
	// std::cout << "getting idx #" << idx << "\n";
	if (_args.empty() || (int)_args.size() <= idx)
	{
		std::cout << "EMPTY!" << std::endl;
		return (std::string());
	}
	return (_args.at(idx));
}

std::string const & Message::get_sender(void) const
{
	return (this->sender);
}

std::string const & Message::print_message(void) const
{
	std::cout	<< "fd: [" << _fd << "]\n"
				<< "prefix: [" << this->prefix << "]\n"
				<< "CMD: [" << this->CMD << "]\n"
				<< "sender: [" << this->sender << "]\n"
				<< "receiver: [" << this->receiver << "]\n"
				<< "postfix: [" << this->postfix << "]\n"
				<< "raw: [" << this->_raw << "]\n"
				<< "legit: [" << this->_legit << "]\n"
				<< "args: [";
	for (int i = 0; i < this->_args.size(); i++)
		std::cout << this->_args[i] << "; ";
	std::cout << std::endl;
	//return (this->CMD + " " + this->sender + " " + this->receiver + " " + this->content + "\r\n")
	return (_raw);
}

int const & Message::get_fd(void) const
{
	return (_fd);
}

void	Message::set_cmd(std::string cmd)
{
	this->CMD = cmd;
}

void	Message::add_arg(char *arg)
{
	std::string temp;

	if (arg == NULL)
		temp = std::string();
	else
		temp = std::string(arg);
	// std::cout << "added arg: [" << arg << "]" << std::endl;
	_args.push_back(arg);
}

std::vector<std::string>	Message::get_arg_vector(void) const
{
	return(this->_args);
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

void	Message::set_receiver(std::string receiver)
{
	this->receiver = receiver;
}

bool	Message::receiver_is_channel(void) const
{
	std::string temp = this->get_receiver();
	if (temp[0] == '#' || temp[0] == '&')
		return (true);
	return (false);
}
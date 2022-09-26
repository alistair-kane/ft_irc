#include "Message.hpp"

Message::Message(int const & fd, char *input) : _fd(fd), _raw(input)
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
	this->content = other.content;

	this->_raw = other._raw;
	this->_fd = other._fd;
	this->_legit = other._legit;
}

Message const & Message::operator=(Message const & rhs)
{
	this->~Message();
	new (this) Message(rhs);
	return (*this);
}

Message::~Message(void)
{}

void	Message::parse(void)
{
	char	*line;
	char	*sgmt;
	int		idx;
	// std::cout << "msg to parse: " << _raw << std::endl;
	line = strtok(_raw, "\n");
	while (line != NULL)
	{
		std::cout << "line: " << line << std::endl;	
		sgmt = strtok(line, " ");
		idx = 0;
		while (sgmt != NULL)
		{
			// need to do other checks etc
			if (idx == 0)
				this->CMD = sgmt;
			else if (idx > 0)
				this->arg = sgmt;
			std::cout << "sep sgmt: " << sgmt << std::endl;
			sgmt = strtok(NULL, " ");
			idx++;
		}
		line = strtok(NULL, "\n");
	}
}

std::string const & Message::get_cmd(void) const
{
	return (this->CMD);
}

std::string const & Message::get_arg(void) const
{
	return (this->arg);
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

// int Message::get_len(void) const
// {
// 	return (_raw.length());
// }

// char const * Message::get_raw(void) const
// {
// 	return (_raw.c_str());
// }

std::string const & Message::get_receiver(void) const
{
	return (this->receiver);
}

bool	Message::receiver_is_channel(void) const
{
	if (receiver[0] == '#')
		return (true);
	return (false);
}
#include "Message.hpp"

Message::Message(int const & fd, char const * input)
{
	this->fd = fd;
	this->raw = input;
	this->complete = false;
	if (this->raw.find("\r\n") != std::string::npos)
		this->complete = true;
}

Message::Message(Message const & other)
{
	this->CMD = other.CMD;
	this->sender = other.sender;
	this->receiver = other.receiver;
	this->content = other.content;
	this->raw = other.raw;
	this->fd = other.fd;
	this->complete = other.complete;
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
	std::string	tmp = this->raw;
	size_t start_pos = 0, end_pos = 0;
	if (tmp[0] == ':')
	{
		end_pos = tmp.find(' ', 0);
		this->prefix = tmp.substr(0, end_pos);
		//std::cout << "prefix: " << this->prefix << "\n";
		tmp.erase(0, end_pos + 1);
		std::cout << "tmp: [" << tmp << "]\n";
	}
	//std::cout << "tmp: [" << tmp << "]\n";
	//start_pos = tmp.find(' ');
	end_pos = tmp.find(' ', 0);
	this->CMD = tmp.substr(0, end_pos);
	//std::cout << "CMD: " << this->CMD << "\n";
	tmp.erase(0, end_pos + 1);
	std::cout << "tmp: [" << tmp << "]\n";
	start_pos = tmp.rfind(" :");
	std::cout << "start_pos: " << start_pos << "\n";
	if (start_pos != 0 && start_pos <= tmp.length())
	{
		//end_pos = raw.rfind(" ");
		this->postfix = tmp.substr(start_pos + 1);
		//std::cout << "postfix: " << this->postfix << "\n";
		tmp.erase(start_pos);
		std::cout << "tmp: [" << tmp << "]\n";
	}
	this->arg = tmp;
}

std::string const & Message::get_cmd(void) const
{
	return (this->CMD);
}

std::string const & Message::get_arg(void) const
{
	return (this->arg);
}

std::string const & Message::print_message(void) const
{
	std::cout	<< "fd: [" << this->fd << "]\n"
				<< "prefix: [" << this->prefix << "]\n"
				<< "CMD: [" << this->CMD << "]\n"
				<< "arg: [" << this->arg << "]\n"
				<< "postfix: [" << this->postfix << "]\n";
	//return (this->CMD + " " + this->sender + " " + this->receiver + " " + this->content + "\r\n");
	return (this->raw);
}

int const & Message::get_fd(void) const
{
	return (this->fd);
}

int Message::get_len(void) const
{
	return (raw.length());
}

char const * Message::get_raw(void) const
{
	return (raw.c_str());
}

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
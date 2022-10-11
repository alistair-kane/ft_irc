#pragma once

#include <iostream>
#include <stdio.h>
#include <cstring>

// should be renamed to messages?

class Message
{
	public:

		Message(int const & fd);
		Message(int const & fd, std::string input);
		Message(Message const & other);
		Message const & operator=(Message const & rhs);
		~Message(void);

		void				parse(void);
		std::string const	&get_cmd(void) const;
		// std::string const & print_message(void) const;
		int const 			&get_fd(void) const;
		// int get_len(void) const;
		void				set_cmd(std::string cmd);
		void				set_arg(std::string _arg);
		const char 			*raw_msg(void) const;
		std::string const 	&get_receiver(void) const;
		bool				receiver_is_channel(void) const;
		std::string const 	&get_arg(void) const;
		size_t				msg_len(void) const;

	private:
		std::string	prefix;
		std::string	postfix;
		std::string	CMD;
		std::string	sender;
		std::string	receiver;
		std::string	_arg;
		int 		_fd;
		std::string _raw;
		bool		_legit;
};
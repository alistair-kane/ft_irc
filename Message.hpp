#pragma once

#include <iostream>
#include <stdio.h>
#include <cstring>
#include <vector>

// should be renamed to messages?

class Message
{
	public:

		Message(std::string input);
		Message(int const & fd);
		Message(int const & fd, std::string input);
		Message(Message const & other);
		Message const & operator=(Message const & rhs);
		~Message(void);

		void				parse(void);
		std::string const	&get_cmd(void) const;
		std::string		 	get_arg(int idx) const;
		std::string const	&get_sender(void) const;
		void				set_receiver(std::string receiver);
		// std::string const & print_message(void) const;
		int const 			&get_fd(void) const;
		// int get_len(void) const;
		void				set_cmd(std::string cmd);
		void				add_arg(char *_arg);
		void				set_sender(std::string _sender);
		const char 			*raw_msg(void) const;
		std::string const 	&get_receiver(void) const;
		bool				receiver_is_channel(void) const;
		size_t				msg_len(void) const;

	private:
		std::string					prefix;
		std::string					postfix;
		std::string					CMD;
		std::string					sender;
		std::string					receiver;
		std::vector<std::string>	_args;
		int							_fd;
		std::string					_raw;
		bool						_legit;
};
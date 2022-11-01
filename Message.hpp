#pragma once

#include <iostream>
#include <stdio.h>
#include <cstring>
#include <vector>

// should be renamed to messages?

class Message
{
	public:

		/* Constructors and Destructor */
		Message(std::string input);
		Message(int const & fd);
		Message(int const & fd, std::string input);
		Message(Message const & other);
		Message const & operator=(Message const & rhs);
		~Message(void);

		void				parse(void);
		// std::string const & print_message(void) const;
		// int get_len(void) const;
		void				add_arg(char *_arg);
		const char 			*raw_msg(void) const;
		bool				receiver_is_channel(void) const;
		size_t				msg_len(void) const;
		
		/* Getters and Setters */
		int const 			&get_fd(void) const;
		std::string const	&get_cmd(void) const;
		void				set_cmd(std::string cmd);
		std::string		 	get_arg(int idx) const;
		std::string const	&get_sender(void) const;
		void				set_sender(std::string _sender);
		std::string const 	&get_receiver(void) const;
		void				set_receiver(std::string receiver);

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
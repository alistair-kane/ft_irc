#pragma once

#include <iostream>

class Client
{
	public:
		//Client(void);
		Client(int const & fd);
		Client(std::string const & nickname);
		Client(Client const & other);
		Client & operator=(Client const & rhs);
		~Client(void);

		bool 		operator<(const Client & rhs) const;

		void		set_nickname(std::string const & nickname);
		std::string const & get_nickname(void) const;
		void		clear_message_buffer(void);
		void		append_message_buffer(char const * input);
		void		append_message_buffer(std::string const & input);
		std::string const & get_message_buffer(void) const;

	private:
		int	fd;
		std::string	nickname;
		std::string	username;
		std::string	message_buffer;
};
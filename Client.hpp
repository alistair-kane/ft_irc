#pragma once

#include <iostream>

class Client
{
	public:
		//Client(void);
		Client(int const & fd);
		Client(std::string const & nickname);
		Client(int const & fd, std::string const & nickname);

		Client(Client const & other);
		Client & operator=(Client const & rhs);
		~Client(void);

		bool 		operator<(const Client & rhs) const;

		// bool				get_auth(void) const;
		// void				set_auth(void);
		std::string const	&get_nickname(void) const;
		void				set_nickname(std::string const &nickname);
		std::string const	&get_username(void) const;
		void				set_username(std::string const &username);
		std::vector<std::string>	&get_channel_list(void) const;
		void						add_to_channel_list(std::string const &channel_name);

		void		clear_message_buffer(void);
		void		append_message_buffer(char const * input);
		void		append_message_buffer(std::string const & input);
		std::string const & get_message_buffer(void) const;

	private:
		int			fd;
		// bool		auth;
		std::string	nickname;
		std::string	username;
		std::string	message_buffer;
		std::vector<std::string>	channel_list;
};
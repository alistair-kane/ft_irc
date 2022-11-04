#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

class Client
{
	public:

		/* Constructors and Destructor */
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

		void				clear_message_buffer(void);
		void				append_message_buffer(char const *input);
		void				append_message_buffer(std::string const &input);
		std::string const	&get_message_buffer(void) const;

		/* Getters and Setters */
		std::string const			&get_nickname(void) const;
		void						set_nickname(std::string const &nickname);
		std::string const			&get_username(void) const;
		void						set_username(std::string const &username);
		// bool						get_pass_auth(void) const;
		// void						set_pass_auth(void);
		bool						get_nick_auth(void) const;
		void						set_nick_auth(bool val);
		bool						get_user_auth(void) const;
		void						set_user_auth(bool val);
		std::vector<std::string>	&get_channel_list(void);
		void						add_to_channel_list(std::string const &channel_name);
		void						remove_from_channel_list(std::string const &channel_name);

	private:
		int			fd;
		// bool		pass_auth;
		bool		nick_auth;
		bool		user_auth;
		std::string	nickname;
		std::string	username;
		std::string	message_buffer;
		std::vector<std::string>	channel_list;
};
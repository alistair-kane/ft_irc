#pragma once

#include <iostream>
#include <set>
#include <map>
#include "Message.hpp"
#include "Client.hpp"

class Channel
{	
	public:
		/* Constructors and Destructor */
		Channel(std::string const &channel_name, int const &fd);
		~Channel();

		/* Restriction list operations */
		void						ban_user(std::string const &nick);
		void						unban_user(std::string const &nick);
		void						can_talk_user(std::string const &nick);
		void						cant_talk_user(std::string const &nick);

		/* Privacy operations */
		void				invite_user(std::string const &nick);
		bool				is_channel_private(void) const;
		void				set_channel_private(bool const &yes);
		bool				is_channel_secret(void) const;
		void				set_channel_secret(bool const &yes);
		bool				is_channel_inviteonly(void) const;
		void				set_channel_inviteonly(bool const &yes);
		bool				is_channel_topic_settable(void) const;
		void				set_channel_topic_settable(bool const &yes);
		bool				is_channel_inside_only(void) const;
		void				set_channel_inside_only(bool const &yes);
		bool				is_channel_moderated(void) const;
		void				set_channel_moderated(bool const &yes);
		bool				is_channel_limited(void) const;
		void				set_channel_limited(bool const &yes);
		
		/* Member operations */
		void								add_member(int const &fd, std::string const &nick);
		void								remove_member(int const & fd);
		
		/* Operator operations */
		void								add_operator(int const &fd);
		void								remove_operator(int const &fd);

		/* Getters and Setters */
		std::set<std::string> const			&get_ban_list(void) const;
		std::map<int, std::string> const	&get_member_list(void) const;
		std::set<int> const					&get_operator_list(void) const;
		std::string const					&get_channel_topic(void) const;
		void								set_channel_topic(std::string const &input);
		void								set_limit(int l);
		void								set_key(std::string k);

		bool			is_member(int const &fd);
		bool			is_operator(int const &fd);
		bool			can_client_join(Client *client_to_add);


private:
	bool						is_private_channel;
	bool						is_secret_channel;
	bool						is_inviteonly_channel;
	bool						is_topic_settable; // by operator only
	bool						is_inside_only_chanel;
	bool						is_moderated_channel;
	bool						is_limited_channel;
	std::set<int>				operator_list;
	std::string					channel_name;
	std::string					channel_topic;
	std::map<int, std::string>	member_list;
	std::set<std::string>		ban_list;
	std::set<std::string>		can_talk_list; // on moderated channel
	std::set<std::string>		invite_list;
	int							user_limit;
	std::string					key;
};

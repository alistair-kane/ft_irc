#pragma once

#include <iostream>
#include <set>
#include <map>
#include "Message.hpp"

class Channel
{	
	public:
		/* Constructors and Destructor */
		Channel(std::string const &channel_name, int const &fd);
		~Channel();

		/* Ban operations */
		void						ban_user(std::string const &nick);
		void						unban_user(std::string const &nick);

		/* Invite operations */
		void				invite_user(std::string const &nick);
		bool				is_channel_private(void) const;
		void				set_channel_private(bool const &invite);
		
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

 
private:
	bool						is_private_channel;
	std::set<int>				operator_list;
	std::string					channel_name;
	std::string					channel_topic;
	std::map<int, std::string>	member_list;
	std::set<std::string>		ban_list;
};

#pragma once

#include <iostream>
#include <set>
#include <map>
#include "Message.hpp"

class Channel
{	
public:
	Channel();
	~Channel();

		std::map<int, std::string> const	&get_member_list(void) const;
		void								add_member(int const &fd, std::string const &nick);
		void								remove_member(int const & fd);

		std::set<std::string> const	&get_ban_list(void) const;
		void						ban_user(std::string const & nick);
		void						unban_user(std::string const & nick);

		void				invite_user(std::string const & nick);
		
		bool				is_channel_private(void) const;
		void				set_channel_private(bool const & invite);
		
		std::string const	&get_channel_topic(void) const;
		void				set_channel_topic(std::string const & input);

 
private:
	bool						is_private_channel;
	std::string					channel_name;
	std::string					channel_topic;
	std::map<int, std::string>	member_list;
	std::set<std::string>		ban_list;
};

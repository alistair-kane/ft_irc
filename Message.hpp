#include <iostream>

class Message
{
	public:

		Message(int const & fd, char const * input);
		Message(Message const & other);
		Message const & operator=(Message const & rhs);
		~Message(void);

		void	parse(void);
		std::string const & get_cmd(void) const;
		std::string const & print_message(void) const;
		int const & get_fd(void) const;
		int get_len(void) const;
		char const * get_raw(void) const;
		std::string const & get_receiver(void) const;
		bool	receiver_is_channel(void) const;
		std::string const & get_arg(void) const;

	private:
		std::string	prefix;
		std::string	postfix;
		std::string	CMD;
		std::string	sender;
		std::string	receiver;
		std::string	content;
		std::string	arg;
		int fd;
		std::string	raw;
		bool	complete;
};
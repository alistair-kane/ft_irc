#include "Server.hpp"

bool	isInt(char *userInput)
{
	for (int i = 1; userInput[i]; ++i)
	{
		if (isdigit(userInput[i]) == false)
			return (false);
	}
	return (true);
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Invalid number of arguments!\n <port> <password>\n";
		exit(-1);
	}
	if (isInt(argv[1]) == false)
	{
		std::cout << "Invalid port as first argument!\n 1024 - 65535\n";
		exit(-1);
	}

	Server IRC_Server(std::atoi(argv[1]), argv[2]);
	IRC_Server.init_server();
	IRC_Server.start_server();
}
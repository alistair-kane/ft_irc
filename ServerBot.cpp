#include "Server.hpp"

void	Server::bot_check(int fd, Message const &msg)
{
    std::string content = msg.raw_msg();
    char const *ranf = "Anna is afraid of dogs.";
    char const *rtf = "Tim likes to be creative with simple decor";
    char const *ralf = "Alistair likes to eat peanut butter from the jar, like ice-cream";

	if (content.find("alistair") != std::string::npos)
    {
        send(fd, ralf, strlen(ralf), MSG_DONTWAIT);
    }
    else if (content.find("tim") != std::string::npos)
    {
        send(fd, rtf, strlen(rtf), MSG_DONTWAIT);
    }
    else if (content.find("anna") != std::string::npos)
    {
        send(fd, ranf, strlen(ranf), MSG_DONTWAIT);
    }
}

#include "Server.hpp"

void	Server::bot_check(int fd, Message const &msg)
{
    std::string content = msg.raw_msg();
    // maybe make vector of random facts xd
    char const *raf = "random alistair fact";
    char const *rtf = "random tim fact";

	if (content.find("alistair") != std::string::npos)
    {
        send(fd, raf, strlen(raf), MSG_DONTWAIT);
    }
    else if (content.find("tim") != std::string::npos)
    {
        send(fd, rtf, strlen(rtf), MSG_DONTWAIT);
    }
}

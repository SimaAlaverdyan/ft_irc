#include "Bot.hpp"

int main(int argc, char const *argv[])
{

    if (argc < 5)
    {
        std::cout << "Usage: bot <host> <port> <pass> <nick>" << std::endl;
        return 1;
    }
    std::string host = argv[1];
    int port = std::atoi(argv[2]);
    std::string pass = argv[3];
    std::string nick = argv[4];

    Bot bot(host, port, pass, nick);

    bot.run_bot();

    return 0;
}

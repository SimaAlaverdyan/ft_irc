#pragma once

class Command;

#include <map>
#include <sstream>
#include <vector>

#include "Command.hpp"
class Server;
class User;


class CommandHandler
{
private:
    Server *_server;
    std::map<std::string, Command*> _commands;

public:
    CommandHandler(Server *server);
    ~CommandHandler();

    void    invoke(User* user);
};

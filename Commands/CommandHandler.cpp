#include "CommandHandler.hpp"

CommandHandler::CommandHandler(Server *server) : _server(server)
{
    _commands["PASS"] = new Pass(_server, false);
    _commands["NICK"] = new Nick(_server, false);
    _commands["USER"] = new Users(_server, false);
    _commands["QUIT"] = new Quit(_server, false);
    _commands["PING"] = new Ping(_server);
    _commands["PONG"] = new Pong(_server);
    _commands["JOIN"] = new Join(_server);
    _commands["PART"] = new Part(_server);
    _commands["KICK"] = new Kick(_server);
    _commands["MODE"] = new Mode(_server);
    _commands["WHO"] = new Who(_server);
    _commands["INVITE"] = new Invite(_server);
    _commands["PRIVMSG"] = new Privmsg(_server);
    _commands["NOTICE"] = new Notice(_server);
}

CommandHandler::~CommandHandler()
{
    for (std::map<std::string, Command*>::iterator it = _commands.begin(); it != _commands.end(); it++)
        delete it->second;
}

void CommandHandler::invoke(User *user)
{
    std::string line;
    if (user->buf.find("\r\n") != std::string::npos)
    {
        line = user->buf.substr(0, user->buf.find("\r\n"));
        user->buf.erase(0, line.size() + 2);
    }
    else
    {
        line = user->buf.substr(0, user->buf.find("\n"));
        user->buf.erase(0, line.size() + 1);
    }
    std::string name = line.substr(0, line.find(" "));

    try
    {
        Command *command = _commands.at(name);

        std::vector<std::string> arguments;

        std::string buf;
        std::stringstream ss(line.substr(name.length(), line.length()));
        line.substr(name.size(), line.size());

        while (ss >> buf)
            arguments.push_back(buf);
        if (!user->is_registered() && command->authRequired())
        {
            user->reply(ERR_NOTREGISTERED(user->get_nickname()));
            return;
        }
        command->execute(user, arguments);
    }
    catch (const std::out_of_range &e)
    {
        user->reply(ERR_UNKNOWNCOMMAND(user->get_nickname(), name));
    }

    if (user->buf.size())
        invoke(user);
}
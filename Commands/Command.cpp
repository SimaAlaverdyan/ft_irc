#include "Command.hpp"

void Pass::execute(User *user, std::vector<std::string> arg)
{
    if (user->is_registered())
    {
        user->reply(ERR_ALREADYREGISTERED(user->get_nickname()));
        return;
    }
    if (arg.empty())
    {
        user->reply(ERR_NEEDMOREPARAMS(user->get_nickname(), "PASS"));
        return;
    }
    if (_server->get_password() != arg[0].substr(arg[0][0] == ':' ? 1 : 0))
    {
        user->reply(ERR_PASSWDMISMATCH(user->get_nickname()));
        return;
    }
    user->set_pass();
    user->registration();
};

void Nick::execute(User *user, std::vector<std::string> arg)
{
    if (arg.empty())
    {
        user->reply(ERR_NEEDMOREPARAMS(user->get_nickname(), "NICK"));
        return;
    }
    if (!user->get_pass())
    {
        user->reply(ERR_ALREADYREGISTERED(user->get_nickname()));
        return;
    }
    std::string nickname = arg[0];
    if (!is_valid(nickname))
    {
        user->reply(ERR_ERRONEUSNICKNAME(user->get_nickname(), nickname));
        return;
    }
    User *new_user = _server->get_user(nickname); // add get_user to the server.hpp
    if (new_user && new_user != user)
    {
        user->reply(ERR_NICKNAMEINUSE(user->get_nickname(), nickname));
        return;
    }
    _server->set_user(user, nickname, user->get_fd());
    user->set_nickname(nickname);
    if (!user->is_registered())
        user->registration();
};

void Users::execute(User *user, std::vector<std::string> arg)
{
    if (user->is_registered() || !user->get_pass())
    {
        user->reply(ERR_ALREADYREGISTERED(user->get_nickname()));
        return;
    }
    if (arg.size() < 4)
    {
        user->reply(ERR_NEEDMOREPARAMS(user->get_nickname(), "USER"));
        return;
    }
    std::string username = arg[0];
    std::string realname = arg[3].substr(arg[3][0] == ':' ? 1 : 0);
    for (size_t i = 4; i < arg.size(); ++i)
        realname.append(" " + arg[i]);

    user->initialize(username, realname);
    user->registration();
}

void Privmsg::execute(User *user, std::vector<std::string> arg)
{
    if (arg.empty())
    {
        user->reply(ERR_NOTEXTTOSEND(user->get_nickname()));
        return;
    }
    std::string target = arg[0];
    std::string message = arg[1].substr(arg[1][0] == ':' ? 1 : 0);
    for (size_t i = 2; i < arg.size(); ++i)
        message.append(" " + arg[i]);
    if (target[0] == '#' || target[0] == '&')
    {
        Channel *channel = _server->get_channel(target); // add get_channel to server.cpp
        if (!channel)
        {
            user->reply(ERR_NOSUCHNICK(user->get_nickname(), target));
            return;
        }
        if (!channel->is_exist(user))
        {
            user->reply(ERR_CANNOTSENDTOCHAN(user->get_nickname(), target));
            return;
        }
        channel->ft_send(user, message, "PRIVMSG");
        return;
    }

    User *new_user = _server->get_user(target); // get_user to server
    if (!new_user)
    {
        user->reply(ERR_NOSUCHNICK(user->get_nickname(), target));
        return;
    }

    std::fstream file;
    std::string path;

    if (!message.find("TFILE"))
    {
        path = "";
        path = message.substr(message.find(" ") + 1, message.size());
        file.open(path, std::ios::in | std::ios::binary);
        
        if(file.is_open())
        {
            std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            message = "TFILE " + contents;
        }
        else
            return; 
        file.close();
    }
    new_user->sendd(RPL_MSG(user->get_print(), "PRIVMSG", target, message));
}

void Notice::execute(User *user, std::vector<std::string> arg)
{
    if (arg.empty())
    {
        user->reply(ERR_NEEDMOREPARAMS(user->get_nickname(), "NOTICE"));
        return;
    }
    if (arg.size() < 2)
    {
        user->reply(ERR_NOTEXTTOSEND(user->get_nickname()));
        return;
    }
    std::string target = arg[0];
    std::string message = arg[1].substr(arg[1][0] == ':' ? 1 : 0);

    for (size_t i = 2; i < arg.size(); ++i)
        message.append(" " + arg[i]);
    if (target[0] == '#')
    {
        Channel *channel = _server->get_channel(target); // add get_channel to the server.hpp
        if (!channel)
        {
            user->reply(ERR_NOSUCHNICK(user->get_nickname(), target));
            return;
        }
        if (!channel->is_exist(user))
        {
            user->reply(ERR_CANNOTSENDTOCHAN(user->get_nickname(), target));
            return;
        }
        channel->ft_send(user, message, "NOTICE");
        return;
    }
    User *users = _server->get_user(target); // add get_user to the server.hpp
    if (!users)
    {
        user->reply(ERR_NOSUCHNICK(user->get_nickname(), target));
        return;
    }
    users->sendd(RPL_MSG(user->get_print(), "NOTICE", target, message));
}

void Ping::execute(User *user, std::vector<std::string> arg)
{
    if (arg.empty())
    {
        user->reply(ERR_NEEDMOREPARAMS(user->get_nickname(), "PING"));
        return;
    }
    user->sendd(RPL_PING(user->get_print(), arg[0]));
}

void Pong::execute(User *user, std::vector<std::string> arg)
{
    if (arg.empty())
    {
        user->reply(ERR_NEEDMOREPARAMS(user->get_nickname(), "PONG"));
        return;
    }
    user->sendd(RPL_PING(user->get_print(), arg[0]));
}

void Join::execute(User *user, std::vector<std::string> arg)
{
    if (arg.empty())
    {
        user->reply(ERR_NEEDMOREPARAMS(user->get_nickname(), "JOIN"));
        return;
    }
    if (arg.size() == 1 && arg[0] == "0")
    {
        user->leave(0);
        return;
    }
    std::string channel = arg[0];
    std::string pass = arg.size() > 1 ? arg[1] : "";
    std::map<std::string, std::string> ch = str_map(channel, pass);
    for (std::map<std::string, std::string>::iterator it = ch.begin(); it != ch.end(); ++it)
    {
        channel = it->first;
        pass = it->second;
        if (channel[0] != '#' && channel[0] != '&')
        {
            user->reply(ERR_BADCHANMASK(user->get_nickname(), channel));
            return;
        }
        Channel *new_channel = _server->get_channel(channel); // add get_channel to the server.hpp
        if (!new_channel)
            new_channel = _server->add_channel(channel, pass); // add add_channel to the server.hpp
        else if (new_channel->size_of_users() == 0)
            new_channel->set_pass(pass);
        if (new_channel->get_pass() != "" && new_channel->get_pass() != pass)
        {
            user->reply(ERR_BADCHANNELKEY(user->get_nickname(), channel));
            return;
        }
        user->join(new_channel);
    }
}

void Part::execute(User *user, std::vector<std::string> arg)
{
    if (arg.empty())
    {
        user->reply(ERR_NEEDMOREPARAMS(user->get_nickname(), "PART"));
        return;
    }
    std::string channel = arg[0];
    Channel *new_channel = _server->get_channel(channel); // add get_channel to the server.hpp
    if (!new_channel)
    {
        user->reply(ERR_NOSUCHCHANNEL(user->get_nickname(), channel));
        return;
    }
    else if (!new_channel->is_exist(user))
    {
        user->reply(ERR_NOTONCHANNEL(user->get_nickname(), channel));
        return;
    }
    new_channel->part(user);
    user->leave(new_channel);
}

void Kick::execute(User *user, std::vector<std::string> arg)
{
    if (arg.empty() || arg.size() < 2)
    {
        user->reply(ERR_NEEDMOREPARAMS(user->get_nickname(), "KICK"));
        return;
    }
    std::string channel = arg[0];
    Channel *new_channel = _server->get_channel(channel); // add get_channel to the server.hpp
    if (!new_channel)
    {
        user->reply(ERR_NOSUCHCHANNEL(user->get_nickname(), channel));
        return;
    }
    else if (!new_channel->is_exist(user))
    {
        user->reply(ERR_NOTONCHANNEL(user->get_nickname(), channel));
        return;
    }
    if (!(new_channel->is_admin(user)))
    {
        user->reply(ERR_CHANOPRIVSNEEDED(user->get_nickname(), channel));
        return;
    }
    std::string username = arg[1];
    User *users = _server->get_user(username); // add get_user to the server.hpp
    if (!users || !new_channel->is_exist(users))
    {
        user->reply(ERR_USERNOTINCHANNEL(user->get_nickname(), username, channel));
        return;
    }
    std::string reason = "No reason specified.";
    if (arg.size() > 2)
    {
        reason = arg[2].substr(arg[2][0] == ':' ? 1 : 0);
        for (size_t i = 3; i < arg.size(); ++i)
            reason.append(" " + arg[i]);
    }
    new_channel->kick(users, reason);
    users->leave(new_channel);
}

void Quit::execute(User *user, std::vector<std::string> arg)
{
    std::string reason;
    if (arg.size())
    {
        reason = arg[0].substr(arg[0][0] == ':' ? 1 : 0);
        for (size_t i = 1; i < arg.size(); i++)
            reason.append(" " + arg[i]);
    }
    user->quit = true;
    user->sendd(RPL_QUIT(user->get_print(), reason));
}

void Mode::execute(User *user, std::vector<std::string> arg)
{
    if (arg.empty())
    {
        user->reply(ERR_NEEDMOREPARAMS(user->get_nickname(), "MODE"));
        return;
    }
    std::string channel = arg[0];
    Channel *new_channel = _server->get_channel(channel); // add get_channel to the server.hpp
    if (!new_channel)
    {
        user->reply(ERR_NOSUCHCHANNEL(user->get_nickname(), channel));
        return;
    }
    else if (!new_channel->is_exist(user))
    {
        user->reply(ERR_NOTONCHANNEL(user->get_nickname(), channel));
        return;
    }
    if (arg.size() == 1)
    {
        if (!(new_channel->get_pass() == ""))
            user->sendd(RPL_MODE(user->get_print(), channel, "+k " + new_channel->get_pass()));
        return;
    }
    std::string mode = arg[1];
    if (arg.size() > 2)
    {
        std::string key = arg[2];
        if (!(new_channel->is_admin(user)))
            user->reply(ERR_CHANOPRIVSNEEDED(user->get_nickname(), channel));
        else if (mode == "+k")
        {
            user->sendd(RPL_MODE(user->get_print(), channel, "+k " + key));
            new_channel->set_pass(key);
        }
        else if (mode == "-k" && new_channel->get_pass() == key)
        {
            user->sendd(RPL_MODE(user->get_print(), channel, "-k "));
            new_channel->set_pass("");
        }
    }
}

void Who::execute(User *user, std::vector<std::string> arg)
{
    if (arg.empty())
    {
        user->reply(ERR_NEEDMOREPARAMS(user->get_nickname(), "WHO"));
        return;
    }
    std::string target = arg[0];

    if (target[0] == '#' || target[0] == '&')
    {
        Channel *channel = _server->get_channel(target);
        if (!channel)
        {
            user->reply(ERR_NOSUCHNICK(user->get_nickname(), target));
            return;
        }
        if (!channel->is_exist(user))
        {
            user->reply(ERR_NOTONCHANNEL(user->get_nickname(), target));
            return;
        }
        channel->who_is_this_guy(user);
        return;
    }
    User *new_user = _server->get_user(target); // get_user
    if (!user)
    {
        user->reply(ERR_NOSUCHNICK(user->get_nickname(), target));
        return;
    }
    user->sendd(RPL_WHOREPLY(user->get_nickname(), "*", new_user->get_username(), new_user->get_hostname(), new_user->get_nickname(), "H", new_user->get_realname()));
    user->sendd(RPL_ENDOFWHO(user->get_nickname(), target));
}

void Invite::execute(User *user, std::vector<std::string> arg)
{
    if (arg.empty() || arg.size() < 2)
    {
        user->reply(ERR_NEEDMOREPARAMS(user->get_nickname(), "WHO"));
        return;
    }
    std::string username = arg[0];
    User *new_user = _server->get_user(username); // add get_user to the server.hpp
    if (!new_user)
    {
        user->reply(ERR_NOSUCHNICK(user->get_nickname(), username));
        return;
    }
    std::string channel = arg[1];
    Channel *new_channel = _server->get_channel(channel);
    if (!new_channel)
    {
        user->reply(ERR_NOSUCHNICK(user->get_nickname(), channel));
        return;
    }
    if (!new_channel->is_exist(user))
    {
        user->reply(ERR_NOSUCHCHANNEL(user->get_nickname(), channel));
        return;
    }
    if (new_channel->is_exist(user))
    {
        user->reply(ERR_USERONCHANNEL(user->get_nickname(), username, channel));
        return;
    }
    new_user->sendd(RPL_INVITE(user->get_print(), username, channel));
    user->reply(RPL_INVITING(user->get_nickname(), username, channel));
}
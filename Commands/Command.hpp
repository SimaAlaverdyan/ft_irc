#pragma once

class Command;

#include <string>
#include <fstream>

#include "../User.hpp"
#include "../server.hpp"

class Command
{
protected:
    Server *_server;
    bool _authRequired;

public:
    Command(Server *server, bool authRequired = true)
        : _server(server),
          _authRequired(authRequired){};

    virtual ~Command(){};

    bool authRequired() const { return _authRequired; };

    virtual void execute(User *user, std::vector<std::string> arguments) = 0;
};

class Pass : public Command
{
public:
    Pass(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Pass() {}

    void execute(User *user, std::vector<std::string> arguments);
};

class Nick : public Command
{
public:
    Nick(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Nick() {}

    void execute(User *user, std::vector<std::string> arguments);
};

class Users : public Command
{
public:
    Users(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Users() {}

    void execute(User *user, std::vector<std::string> arguments);
};

class Privmsg : public Command
{
public:
    Privmsg(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Privmsg() {}

    void execute(User *user, std::vector<std::string> arguments);
};

class Notice : public Command
{
public:
    Notice(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Notice() {}

    void execute(User *user, std::vector<std::string> arguments);
};

class Ping : public Command
{
public:
    Ping(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Ping() {}

    void execute(User *user, std::vector<std::string> arguments);
};

class Pong : public Command
{
public:
    Pong(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Pong() {}

    void execute(User *user, std::vector<std::string> arguments);
};

class Join : public Command
{
public:
    Join(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Join() {}

    void execute(User *user, std::vector<std::string> arguments);
};

class Part : public Command
{
public:
    Part(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Part() {}

    void execute(User *user, std::vector<std::string> arguments);
};

class Kick : public Command
{
public:
    Kick(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Kick() {}

    void execute(User *user, std::vector<std::string> arguments);
};

class Quit : public Command
{
public:
    Quit(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Quit() {}

    void execute(User *user, std::vector<std::string> arguments);
};

class Mode : public Command
{
public:
    Mode(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Mode() {}

    void execute(User *user, std::vector<std::string> arguments);
};

class Who : public Command
{
public:
    Who(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Who() {}

    void execute(User *user, std::vector<std::string> arguments);
};

class Invite : public Command
{
public:
    Invite(Server *server, bool authRequired = true) : Command(server, authRequired) {}
    ~Invite() {}

    void execute(User *user, std::vector<std::string> arguments);
};
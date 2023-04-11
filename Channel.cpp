#include "Channel.hpp"

Channel::Channel() {}

Channel::~Channel() {}

Channel::Channel(const std::string &name, const std::string &pass)
    : _name(name), _pass(pass)
{
}

std::string Channel::get_name(void)
{
    return (_name);
}

std::vector<User *> Channel::get_users(void)
{
    return (_users);
}

void Channel::join(User *user)
{
    _users.push_back(user);
    for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); it++)
    {
        (*it)->sendd(RPL_JOIN(user->get_print(), _name));
    }
    this->new_admin();
    this->name_of_that_guy(user);
}

void Channel::part(User *user)
{
    std::vector<User *>::iterator tmp;
    for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); it++)
    {
        (*it)->sendd(RPL_PART(user->get_print(), _name));
        if (*it == user)
            tmp = it;
    }
    _users.erase(tmp);
    this->new_admin();
}

void Channel::kick(User *user, const std::string &reason)
{
    std::vector<User *>::iterator tmp;
    for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); it++)
    {
        (*it)->sendd(RPL_KICK(user->get_print(), _name, user->get_nickname(), reason));
        if (*it == user)
            tmp = it;
    }
    _users.erase(tmp);
}

void Channel::ft_send(User *user, const std::string &msg, const std::string &cmd)
{
    for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); it++)
        if (*it != user)
            (*it)->sendd(RPL_MSG(user->get_print(), cmd, _name, msg));
}

bool Channel::is_admin(User *user)
{
    if (!is_exist(user))
        return false;
    return _admin == user;
}

bool Channel::is_exist(User *user)
{
    for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); it++)
        if (*it == user)
            return true;
    return false;
}

void Channel::who_is_this_guy(User *user)
{
    for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); it++)
        user->sendd(RPL_WHOREPLY(user->get_nickname(), _name, (*it)->get_username(), (*it)->get_hostname(), (*it)->get_nickname(), "H", (*it)->get_realname()));
    user->sendd(RPL_ENDOFWHO(user->get_nickname(), _name));
}

void Channel::name_of_that_guy(User *user)
{
    for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); it++)
    {
        if (*it == _admin)
            user->sendd(RPL_NAMREPLY(user->get_nickname(), _name, "@", (*it)->get_nickname()));
        else
            user->sendd(RPL_NAMREPLY(user->get_nickname(), _name, "+", (*it)->get_nickname()));
    }
    user->sendd(RPL_ENDOFNAMES(user->get_nickname(), _name));
}

int Channel::size_of_users(void)
{
    return _users.size();
}

std::string Channel::get_pass(void)
{
    return _pass;
}

void Channel::set_pass(const std::string &pass)
{
    _pass = pass;
}

void Channel::new_admin(void)
{
    if (_users.size() > 0 && _admin != _users[0])
    {
        _admin = _users[0];
        _admin->sendd(RPL_MSG(_admin->get_print(), "", _name, "you are the new admin"));
        this->ft_send(_admin, "is new admin", "");
        for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); it++)
            this->name_of_that_guy(*it);
    }
}
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

class Channel;

#include <vector>
#include "User.hpp"
// #include "ircserv.hpp"

// class User;

class Channel
{
public:
    Channel();
    Channel(const std::string &name, const std::string &pass);
    ~Channel();

    void join(User *user);
    void part(User *user);
    void kick(User *user, const std::string &reason);

    void ft_send(User *user, const std::string &msg, const std::string &cmd);
    // bool is_exist(User *user);
    bool is_admin(User *user);
    bool is_exist(User *user);

    void who_is_this_guy(User *user);
    void name_of_that_guy(User *user);

    std::string get_name(void);
    std::vector<User *>  get_users(void);

    int size_of_users(void);
    std::string get_pass(void);
    void set_pass(const std::string &pass);
    void new_admin(void);

private:
    std::string             _name;
    std::vector<User *>     _users;
    User                    *_admin;

    std::string             _pass;

};

#endif

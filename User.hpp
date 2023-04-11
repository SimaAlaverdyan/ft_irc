#ifndef USER_HPP
# define USER_HPP

class User;

#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <sys/socket.h>

#include "Channel.hpp"
#include "ircserv.hpp"
class Channel;

class User
{
private:
    
    std::string m_nickname;
    std::string m_username;
    std::string m_hostname;
    std::string m_realname;
    bool    m_reg;
    bool    m_pass;
    int     m_fd;

    std::vector<Channel *> m_channel;

public:
    std::string buf;
    bool        quit;

public:
    User(int fd, std::string hostname);
    ~User();

    void initialize(const std::string &username, const std::string &realname);

    int get_fd() const { return m_fd; };
    std::string get_hostname() const { return m_hostname; };
    std::string get_nickname() const { return m_nickname; };
    std::string get_username() const { return m_username; };
    std::string get_realname() const { return m_realname; };
    std::string get_print() const { return (m_nickname + (m_username.empty() ? "" : "!" + m_username) + (m_hostname.empty() ? "" : "@" + m_hostname));}

    void set_hostname(const std::string &hostname) { m_hostname = hostname; };
    void set_nickname(const std::string &nickname) { 
        if(!(m_nickname == ""))
            this->sendd(":" + m_nickname + "NICK" + nickname);
        m_nickname = nickname; 
    };
    void set_username(const std::string &username) { m_username = username; };
    void set_realname(const std::string &realname) { m_realname = realname; };
    
    void registration();
    bool is_registered();

    void set_pass();
    bool get_pass();

    void sendd(const std::string &message);
    void reply(const std::string &message);
    void join(Channel *channel) ;

    void leave(int del);
    void leave(Channel *channel);
}; 

#endif

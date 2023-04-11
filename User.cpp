#include "User.hpp"

User::User(int fd, std::string _hostname)
    : m_nickname(),
      m_username(),
      m_hostname(_hostname),
      m_realname(),
      m_reg(false),
      m_pass(false),
      m_fd(fd) { quit = false; }

User::~User() {}

void User::initialize(const std::string &username, const std::string &realname)
{
    m_username = username;
    m_realname = realname;
}

void User::registration()
{
    if (m_pass && !m_username.empty() && !m_realname.empty() && !m_nickname.empty())
    {
        m_reg = true;
        reply(RPL_WELCOME(m_nickname));
        std::cout << "Registered: " << this->get_print() << std::endl;
    }
}

bool User::is_registered() { return m_reg; }

void User::set_pass() { m_pass = true; }

bool User::get_pass() { return m_pass; }

void User::sendd(const std::string &message)
{
    std::string buf = message + "\r\n";

    if (send(m_fd, buf.c_str(), buf.length(), 0) < 0)
        std::cout << "Error: can't send message to client." << std::endl;
}

void User::reply(const std::string &message)
{
    sendd(":" + get_print() + " " + message);
}

void User::join(Channel *channel)
{
    m_channel.push_back(channel);
    channel->join(this);
}

void User::leave(int del)
{
    (void)del;
    for (std::vector<Channel *>::iterator it = m_channel.begin(); it != m_channel.end(); ++it)
        (*it)->part(this);
    m_channel.clear();
}

void User::leave(Channel *channel)
{
    for (std::vector<Channel *>::iterator it = m_channel.begin(); it != m_channel.end(); ++it)
    {
        if (*it == channel)
        {
            m_channel.erase(it);
            break;
        }
    }
}
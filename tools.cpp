#include <iostream>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <map>

void errors(std::string err)
{
    if (errno)
        perror(err.c_str());
    else
        std::cerr << err << std::endl;
    exit(1);
};

bool is_valid(std::string str)
{
    std::string::iterator c = str.begin();
    if (!std::isalpha(*c))
        return false;
    for (; c != str.end(); ++c)
        if (!std::isprint(*c))
            return false;
    return true;
};

bool is_all_digit(std::string str)
{
    for (std::string::iterator c = str.begin(); c != str.end(); ++c)
        if (!std::isdigit(*c))
            return false;
    return true;
};

std::vector<std::string> ft_split(const std::string &arg)
{
    std::vector<std::string> ret;
    std::string temp;
    size_t argSize = arg.size();
    size_t pos_0 = 0;
    size_t pos;
    while ((pos = arg.find(",", pos_0)) <= argSize)
    {
        temp = arg.substr(pos_0, pos - pos_0);
        ret.push_back(temp);
        temp = "";
        pos_0 = pos + 1;
    }
    temp = arg.substr(pos_0, pos);
    ret.push_back(temp);
    return ret;
}

std::map<std::string, std::string> str_map(std::string &channel, std::string &password)
{
    std::map<std::string, std::string> ret;
    std::vector<std::string> c = ft_split(channel);
    std::vector<std::string> pass = ft_split(password);
    size_t ch = c.size();
    size_t vpas = pass.size();
    for (size_t i = 0; i < ch; i++)
    {

        if (vpas > i)
            ret[c[i]] = pass[i];
        else
            ret[c[i]] = "";
    }
    return ret;
};

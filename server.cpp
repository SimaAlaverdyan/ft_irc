#include "server.hpp"

Server::Server(short port, const std::string& pass)
    :_port(port),
    _pass(pass),
    _commandHandler(new CommandHandler(this))
{
    init_server();
    run();
}

Server::~Server()
{
    for (iterator it = _user.begin(); it != _user.end(); ++it)
    {
        delete it->second;
        close(it->first);
    }
    close(server_fd);
    for (std::map<std::string, Channel*>::iterator it = _channel.begin(); it != _channel.end(); ++it)
    {
        delete it->second;
    }
}

short   Server::getPort() const
{
    return _port;
}

std::string Server::get_password()
{
    return _pass;
}

User* Server::get_user(const std::string& nickname)
{
    try
    {
        return _user.at(_new_user.at(nickname));
    }
    catch(const std::exception& e)
    {
        return NULL;
    }
}

Channel*    Server::get_channel(const std::string& name)
{
    try
    {
        return _channel.at(name);
    }
    catch(const std::exception& e)
    {
        return NULL;
    }
}

Channel*    Server::add_channel(const std::string& name, const std::string& pass)
{
    Channel* newChannel = new Channel(name, pass);

    _channel.insert(std::make_pair(name, newChannel));
    return newChannel;
}

void    Server::set_user(User* client, const std::string& nick, int fd)
{
    std::string old_nick = client->get_nickname();
    if (!(old_nick ==  ""))
        for (iterator it = _user.begin(); it != _user.end(); ++it)
            if (it->second != client)
                it->second->sendd(":" + old_nick + " NICK " + nick);
    _new_user.erase(old_nick);
    _new_user.erase(nick);
    _new_user.insert(std::make_pair(nick, fd));
}

/****************[init server]****************/
void    Server::init_server()
{
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        errors("socket failed");

    if (setsockopt(server_fd, SOL_SOCKET, \
        SO_REUSEADDR, &opt, sizeof(opt)))
        errors("setsockopt");
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
        errors("bind failed");

    if (listen(server_fd, 100) < 0)
        errors("listen");

    fcntl(server_fd, F_SETFL, O_NONBLOCK);
}

/****************[make select]****************/
void    Server::run()
{
    int     r, nfds = server_fd;
    fd_set  rd, wr, er;
    timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 300000;
    FD_ZERO(&rd);
    FD_ZERO(&wr);
    FD_ZERO(&er);
    for(;;)
    {
        if (_user.size())
        {
            for (iterator it = _user.begin(); it != _user.end(); ++it)
                FD_SET(it->first, &rd);
            nfds = std::max(nfds, _user.rbegin()->first);
        }

        r = select(nfds + 1, &rd, &wr, &er, &tv);
        if (r == -1)
            errors("select");
        else if (r)
        {
            for (iterator it = _user.begin(); it != _user.end(); ++it)
            {
                /*************[writing]*************/
                if (FD_ISSET(it->first, &wr))
                {
                    FD_CLR(it->first, &wr);
                    while (!(it->second->buf).empty())
                        _commandHandler->invoke(it->second);
                    it->second->buf.clear();
                    if ( it->second->quit) {
                        delete_user(it);
                        break ;
                    }
                }
                /*************[reading]*************/
                else if (FD_ISSET(it->first, &rd))
                {
                    FD_CLR(it->first, &rd);
                    if (!ft_buffer(it))
                        break ;
                    FD_SET(it->first, &wr);
                }
            }
        }
        else
            new_user();
    }
}

bool    Server::ft_buffer(iterator& it)
{
    char    buffer[BUF_SIZE];
    int     valread;

    while ((memset(buffer, 0, BUF_SIZE), \
            valread = recv(it->first, buffer, BUF_SIZE, 0)) != -1)
    {
        if (valread == 0)
        {
            delete_user(it);
            return valread;
        }
        it->second->buf.append(buffer);
    }
    return valread;
}

/****************[create new client]****************/
void    Server::new_user()
{
    sockaddr_in client_address;
    socklen_t   client_addrlen = sizeof(client_address);

    memset(&address, 0, client_addrlen);
    int new_socket = accept(server_fd, (struct sockaddr*)&client_address, &client_addrlen);
    if (new_socket == -1)
        return ;
    fcntl(new_socket, F_SETFL, O_NONBLOCK);
    char hostname[NI_MAXHOST];

    getnameinfo((struct sockaddr*)&client_address, sizeof(client_address), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV);
    User* n_user = new User(new_socket, hostname);
    _user.insert(std::make_pair(new_socket, n_user));
    std::cout << "New connection: " << n_user->get_print() << std::endl;
    std::cout << "Users connection: " << _user.size() << std::endl;
}

void    Server::delete_user(iterator& it){

    std::cout << "User disconnected: " << it->second->get_print() << std::endl;

    it->second->leave(0);

    close(it->first);
    _new_user.erase(it->second->get_nickname());

    delete it->second;
    _user.erase(it);
    std::cout << "Users connection: " << _user.size() << std::endl;
}
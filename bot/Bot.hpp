#ifndef BOT_HPP
# define BOT_HPP

# include <sstream>
# include <iostream>
# include <fstream>
# include <string>
# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <netdb.h>
# include <sys/uio.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <fcntl.h>

# define BUFF_SIZE 5000

class Bot
{
private:
    std::string _host;
    int _port;
    std::string _pass;
    std::string _nick;
    std::string m_user;
    
    int bot_socket;
    struct sockaddr_in address;
    
    int bytes_written;
    int bytes_read;
    struct timeval start;
    struct timeval end;
public:
    Bot( std::string host,
         int port,
         std::string pass,
         std::string nick );
    ~Bot();

    void bot_init();
    void run_bot();
    void bot_close();

    void create_socket();
    void create_connection();

    void send_msg(const std::string& data);
    int command_handler(std::string msg);

    std::string parser(std::string &data, std::string *msg);

    std::string UpCase();
    std::string say_hi();
    std::string file_transfer(std::string content);
};


#endif // !BOT_HPPP
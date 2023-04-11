#include "Bot.hpp"

//Utility start
std::string    ltrim(const std::string &s, const std::string &trimed)
{
    size_t start = s.find_first_not_of(trimed);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string    rtrim(const std::string &s, const std::string &trimed)
{
    size_t end = s.find_last_not_of(trimed);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string    trim(const std::string &s, const std::string &trimed) { 
    return rtrim(ltrim(s, trimed), trimed);
}

void  toUpperCase(std::string& str)
{
    for(size_t i = 0 ; i < str.size(); ++i) 
    { str[i] = std::toupper(str[i]); }
}

std::string my_to_string(int num)
{
    std::stringstream stream;
    std::string str;
    stream << num;
    stream >> str;
    return str;
}
//Utility end


Bot::Bot( std::string host,
          int port,
          std::string pass,
          std::string nick ) :
          _host(host), _port(port), _pass(pass), _nick(nick)

{   
}

Bot::~Bot()
{
}

void Bot::create_socket()
{
    if ((bot_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        perror("Socket failed.\n");
        exit(EXIT_FAILURE);
    }
    std::cout<<"Socket Created Successfully.\n";
}

void Bot::create_connection()
{
    if (connect(bot_socket, (struct sockaddr *)&address, sizeof(address)) < 0) { 
        perror("connection attempt failed.\n");
        exit(EXIT_FAILURE);
    }
    std::cout<<"Connection Successfull.\n";
}

void Bot::bot_init()
{
    bytes_read = 0;
    bytes_written = 0;
    create_socket();

    address.sin_family = AF_INET;
    address.sin_port = htons( _port );
    std::cout << _host << std::endl;
    if(inet_pton(AF_INET, _host.c_str(), &address.sin_addr)<=0) { 
        std::cout<<"Invalid address\n";
        exit(EXIT_FAILURE);
    }
}

void Bot::send_msg(const std::string& data)
{
    std::string buffer = data + "\r\n";
    bytes_written += send(bot_socket, buffer.c_str(), strlen(buffer.c_str()), 0);
}

void Bot::run_bot()
{
    bot_init();

    char msg[BUFF_SIZE];
    std::string data;
    gettimeofday(&start, NULL);    

    std::cout << "********Bot********" << std::endl;
    std::cout << "Hi! My name is Sirik" << std::endl;
    std::cout << "Please send one of the options below" << std::endl;
    std::cout << "UPCASE" << std::endl;
    std::cout << "SAYHI" << std::endl;
    std::cout << "TFILE" << std::endl;
    std::cout << "EXIT" << std::endl;
    
    while (1)
    {
        if (connect(bot_socket, (struct sockaddr *)&address, sizeof(address)) != -1)
        {
            fcntl(bot_socket, F_SETFL, O_NONBLOCK);
            send_msg("PASS " + _pass);
            send_msg("NICK " + _nick);
            send_msg("USER " + _nick + " " + my_to_string(_port) + " " + _host + " :Noname");
            
            while (1)
            {
                memset(&msg, 0, sizeof(msg));
                int val = recv(bot_socket, (char*)&msg, sizeof(msg), 0);
                if (val == 0)
                    break; 
                bytes_read += val;
                data = "";
                data.append(msg);
                if (!data.empty())
                {
                    if (command_handler(data))
                        break;
                }
            }
            break;
        }
    }
    gettimeofday(&end, NULL);
    bot_close();
}

void Bot::bot_close()
{
    close(bot_socket);
    std::cout << "********Session********" << std::endl;
    std::cout << "Bytes written: " << bytes_written << std::endl;
    std::cout << "Bytes read: " << bytes_read << std::endl;
    std::cout << "Elapsed time: " << (end.tv_sec - start.tv_sec) << " secs" << std::endl;
    std::cout << "Connection closed" << std::endl;
}

std::string Bot::UpCase()
{
    std::fstream file;
    std::string content;

    file.open("data.txt", std::ios::in | std::ios::binary);
    if(!file.is_open())
    {
        std::cout << "File loading failed, Exititng.\n";
        exit(EXIT_FAILURE);
    }

    file >> content;
    toUpperCase(content);
    
    return content;
}

std::string Bot::parser(std::string &data, std::string *msg){
    m_user = "";
    if (data.find("PRIVMSG") > data.size())
        return "";
    m_user = data.substr(1, data.find("!") - 1);
    std::string temp = data.substr(data.find(":", 1) + 1, data.size());
    std::string command = temp;
    temp = temp.substr(temp.find(":", 0) + 1, temp.size());
    
    if (temp.size() < command.size())
        command = command.substr(0, command.size() - temp.size());
    else
        temp = "";
    temp = trim(temp, " :\r\n\t");
    command = trim(command," :\r\n");
    *msg = temp;
    return command;
}

std::string Bot::say_hi()
{
    return "Hi my name is " + _nick + "\r\n";
}

std::string Bot::file_transfer(std::string content)
{
    std::fstream file;

    file.open("bot.txt", std::ios::out | std::ios::trunc | std::ios::binary);
    
    if(file.is_open())
        std::cout << "File Creted.\n";
    else
        std::cout<<"File creation failed, Exititng.\n";
    
    file<<content;
    file.close();
    return "File Saved.\n";
}

int Bot::command_handler(std::string msg)
{
    std::string txt;
    std::string command = parser(msg, &txt);

    if (m_user.empty() || command.empty())
        return 0;
    if (command == "UPCASE")
        txt = UpCase();
    else if (command == "SAYHI")
        txt = say_hi();
    else if (!command.find("TFILE"))
        txt = file_transfer(command.substr(6, command.size()));
    else if (command == "EXIT")
        return 1;
    else
        txt = "Wrong input\r\n";

    if (!txt.empty())
        send_msg("PRIVMSG " + m_user + " " + txt);
    return 0;
}

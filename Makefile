NAME		= ircserv
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -I.
CPPFLAGS	= -std=c++98
RM			= rm -rf
SRCS		= $(wildcard *.cpp) $(wildcard Commands/*.cpp)
OBJS		= $(SRCS:.cpp=.o)

ifeq (run,$(firstword $(MAKECMDGOALS)))
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
endif

all:		$(NAME)

run:		all
			@echo "runing prog $(NAME)."
			./$(NAME) $(RUN_ARGS)

$(NAME):	$(OBJS)
			$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) -o $(NAME)
			@echo "$(NAME) compiled!"

clean:
			@$(RM) $(OBJS)
			@echo "Cleaned!"

fclean: clean
			@$(RM) $(NAME)
			@echo "All cleaned!"

re: fclean all

server: all
			./$(NAME) 6667 asd
client: 
		nc localhost 6667

.PHONY:		all clean fclean re run server client
NAME		= bot
CPP			= c++
CPPFLAGS	= -Wall -Wextra -Werror -I. -std=c++98
RM			= rm -rf
SRCS		= Bot.cpp main.cpp
OBJS		= $(SRCS:.cpp=.o)

all:		$(NAME)

run:		all
			$(NAME) $(RUN_ARGS)

$(NAME):	$(OBJS)
			$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)
clean:
			$(RM) $(OBJS)

fclean: clean
			$(RM) $(NAME)

re: fclean all


.PHONY:		all clean fclean re run start
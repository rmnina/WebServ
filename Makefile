# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/18 19:01:05 by jdufour           #+#    #+#              #
#    Updated: 2025/01/28 17:07:04 by eltouma          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

CC = c++ -Wall -Wextra -Werror -g3 -std=c++98
MAKEFLAGS += --no-print-directory

GREEN := \e[32m
RESET :=\e[0m

SRCS_PATH = $(shell find src -type d)

SRCS = Config.cpp ConfigStruct.cpp Handler.cpp \
	Server.cpp Signal.cpp CheckRequest.cpp \
	ErrorPage.cpp BuildResponse.cpp Location.cpp main.cpp
	
vpath %.cpp $(foreach dir, $(SRCS_PATH), $(dir):)

OBJS_PATH = obj/

OBJS = $(addprefix $(OBJS_PATH), $(SRCS:%.cpp=%.o))

all: $(NAME)

$(OBJS_PATH)%.o: %.cpp
	@mkdir -p $(OBJS_PATH)
	$(CC) -c $< -o $@

$(NAME): $(OBJS)
	@echo "\nCompilation webserv: $(GREEN)success$(RESET)\n"
	$(CC) $(OBJS) -Inc -o $(NAME)

clean:
	/bin/rm -rf $(OBJS)
	@echo "\nWebserv removed: $(GREEN)success$(RESET)\n"

fclean:	clean
	/bin/rm -rf ${NAME}

re: fclean all

.PHONY: all clean fclean re

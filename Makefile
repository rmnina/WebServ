# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/18 19:01:05 by jdufour           #+#    #+#              #
#    Updated: 2024/11/24 03:38:50 by jdufour          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = WebServ

SRC_DIR = src

SRCS = $(addprefix $(SRC_DIR)/, config/Config.cpp config/ConfigStruct.cpp server/Handler.cpp server/Server.cpp server/Signal.cpp parser/Parser.cpp parser/Response.cpp main.cpp)

OBJ_DIR = obj

OBJ_DIRS = $(addprefix $(OBJ_DIR)/, config server parser)

OBJ = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CXX = c++

CPPFLAGS = -g3 -Wall -Wextra -Werror -std=c++98

RM = rm -rfv

$(OBJ_DIRS):
	@mkdir -p $@

all : $(OBJ_DIRS) $(NAME)

$(NAME) : $(OBJ)
	$(CXX) $(CPPFLAGS) $^ -o $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp | $(OBJ_DIRS)
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean :
	$(RM) $(OBJ)

fclean : clean
	$(RM) $(NAME)

re : fclean all

.PHONY : all clean fclean re
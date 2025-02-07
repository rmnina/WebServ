NAME = webserv

CC = c++ -Wall -Wextra -Werror -g3 -std=c++98
C = gcc -Wall -Wextra -Werror -g3
MAKEFLAGS += --no-print-directory

GREEN := \e[32m
RESET := \e[0m

# Sources paths
SRCS_PATH = $(shell find src -type d)
CGI_PATH = www/cgi

# Main files
SRCS = Config.cpp ConfigStruct.cpp Handler.cpp \
	Server.cpp Signal.cpp CheckRequest.cpp \
	ErrorPage.cpp BuildResponse.cpp Location.cpp main.cpp
	
# CGI files
CGI_SOURCES = random_number.cpp infinite_loop.c # www/cgi/kaamelott.rb)

# Filter cgi by extension
CGI_CPP = $(filter %.cpp, $(CGI_SOURCES))
CGI_C = $(filter %.c, $(CGI_SOURCES))

CGI_BIN_CPP = $(addprefix $(CGI_PATH)/, $(CGI_CPP:%.cpp=%.cgi))
CGI_BIN_C = $(addprefix $(CGI_PATH)/, $(CGI_C:%.c=%.cgi))
CGI_BIN = $(CGI_BIN_CPP) $(CGI_BIN_C)

OBJS_PATH = obj/
OBJS = $(addprefix $(OBJS_PATH), $(SRCS:%.cpp=%.o))

# VPATH pour localiser les fichiers sources
vpath %.cpp $(SRCS_PATH)
vpath %.cpp $(CGI_PATH)
vpath %.c $(CGI_PATH)

# webserv and CGI compilation
all: $(NAME) cgi

# Compilation des fichiers objets
$(OBJS_PATH)%.o: %.cpp
	@mkdir -p $(OBJS_PATH)
	$(CC) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(OBJS) -Inc -o $(NAME)
	@echo "\nCompilation webserv: $(GREEN)success$(RESET)\n"

# CGI compilation
cgi: $(CGI_BIN)

# Règle pour les fichiers CGI .cpp
$(CGI_PATH)/%.cgi: $(CGI_PATH)/%.cpp
	$(CC) $< -o $@
	@echo "\nCompilation CGI $<: $(GREEN)success$(RESET)\n"

$(CGI_PATH)/%.cgi: $(CGI_PATH)/%.c
	$(C) $< -o $@
	@echo "\nCompilation CGI $<: $(GREEN)success$(RESET)\n"

# Nettoyage des objets et fichiers CGI
clean:
	/bin/rm -rf $(OBJS) 
	@echo "\nClean obj: $(GREEN)success$(RESET)\n"
	/bin/rm -rf $(CGI_BIN)
	@echo "\nClean CGI: $(GREEN)success$(RESET)\n"

fclean: clean
	/bin/rm -rf $(NAME)
	@echo "\nClean webserv: $(GREEN)success$(RESET)\n"

re: fclean all

.PHONY: all clean fclean re cgi


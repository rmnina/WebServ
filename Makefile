NAME = webserv

CC = c++ -Wall -Wextra -Werror -g3 -std=c++98
MAKEFLAGS += --no-print-directory

GREEN := \e[32m
RESET := \e[0m

# Sources paths
SRCS_PATH = $(shell find src -type d)
#CGI_PATH = $(shell find www/cgi -type d)
CGI_PATH = www/cgi

# Fichiers sources principaux
SRCS = Config.cpp ConfigStruct.cpp Handler.cpp \
	Server.cpp Signal.cpp CheckRequest.cpp \
	ErrorPage.cpp BuildResponse.cpp Location.cpp main.cpp
	
#vpath %.cpp $(foreach dir, $(SRCS_PATH), $(dir):) pas sur


# CGI files
#CGI_SOURCES = www/cgi/random_number.cpp # www/cgi/kaamelott.rb
CGI_SOURCES = random_number.cpp # www/cgi/kaamelott.rb)
CGI_BIN = $(addprefix $(CGI_PATH)/, $(CGI_SOURCES:%.cpp=%.cgi))

OBJS_PATH = obj/
OBJS = $(addprefix $(OBJS_PATH), $(SRCS:%.cpp=%.o))

# VPATH pour localiser les fichiers sources
vpath %.cpp $(SRCS_PATH)
vpath %.cpp $(CGI_PATH)

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


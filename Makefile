NAME = webserv
CC = c++

INCLUDES_DIR = includes

SRCS_DIR = srcs

SRCS =	$(addprefix $(SRCS_DIR)/, Cgi.cpp) \
		$(addprefix $(SRCS_DIR)/, Cluster.cpp) \
		$(addprefix $(SRCS_DIR)/, Client.cpp) \
		$(addprefix $(SRCS_DIR)/, Config.cpp) \
		$(addprefix $(SRCS_DIR)/, Server.cpp) \
		$(addprefix $(SRCS_DIR)/, tools.cpp) \
		$(addprefix $(SRCS_DIR)/, webserv.cpp) \
		$(addprefix $(SRCS_DIR)/, Request.cpp) \
		$(addprefix $(SRCS_DIR)/, Response.cpp)

OBJS = $(SRCS:%.cpp=$(OBJS_DIR)/%.o)

OBJS_DIR = objs

CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

# ANSI color codes
GREEN = \033[1;32m
YELLOW = \033[1;33m
WHITE = \033[1;97m
BLUE = \033[1;34m
CYAN = \033[1;36m
RED = \033[1;31m
RESET = \033[0m

COL1 = \033[38;2;255;204;153m # Pastel orange
COL2 = \033[38;2;255;214;140m # Light pastel orange
COL3 = \033[38;2;255;224;128m # Pastel yellow-orange
COL4 = \033[38;2;255;234;115m # Pastel orange-yellow
COL5 = \033[38;2;255;244;102m # Light pastel yellow
COL6 = \033[38;2;255;255;153m # Pastel yellow

all: $(NAME)

$(OBJS_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "$(CYAN)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@ -I $(INCLUDES_DIR)

$(NAME): $(OBJS)
	@echo "\n$(YELLOW)Linking objects...$(RESET)"
	@$(CC) $(OBJS) -o $(NAME)
	@echo "$(BLUE)Progress: 100%$(RESET)"
	@echo "$(GREEN)Compilation complete!$(RESET)\n"
	@echo "$(COL1)██╗    ██╗███████╗██████╗ ███████╗███████╗██████╗ ██╗   ██╗"
	@echo "$(COL2)██║    ██║██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██║   ██║"
	@echo "$(COL3)██║ █╗ ██║█████╗  ██████╔╝███████╗█████╗  ██████╔╝██║   ██║"
	@echo "$(COL4)██║███╗██║██╔══╝  ██╔══██╗╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝"
	@echo "$(COL5)╚███╔███╔╝███████╗██████╔╝███████║███████╗██║  ██║ ╚████╔╝ "
	@echo "$(COL6) ╚══╝╚══╝ ╚══════╝╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  $(RESET)\n"

clean:
	@rm -rf $(OBJS_DIR)
	@echo "$(RED)Cleaned object files.$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)Removed executable.$(RESET)"

re: fclean all

.PHONY: all clean fclean re

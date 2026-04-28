NAME = webserv

CXX = c++

CXXFLAGS = -Wall -Werror -Wextra -std=c++98
# -g -O0 -fsanitize=address

SRC =main.cpp \
	Server.cpp \
	RequestHandler.cpp \
	Router.cpp \
	HttpRequest.cpp \
	CgiHandler.cpp \
	HttpResponse.cpp \
	UploadHandler.cpp \
	FileHandler.cpp \
	Config.cpp \
	ConfigParse.cpp

OBJ_FOLDER = obj_files

OBJ_SRC = $(patsubst %.cpp, $(OBJ_FOLDER)/%.o, $(SRC))

#color
GREEN = \033[0;32m
CYAN = \033[0;36m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJ_SRC)
	$(CXX) $(CXXFLAGS) $(OBJ_SRC) -o $(NAME)
	@clear
	@echo "${CYAN}==============================================================================${RESET}"
	@echo "${CYAN}░██       ░██            ░██                                                  ${RESET}"
	@echo "${CYAN}░██       ░██            ░██                                                  ${RESET}"
	@echo "${CYAN}░██  ░██  ░██  ░███████  ░████████   ░███████   ░███████  ░██░████ ░██    ░██ ${RESET}"
	@echo "${CYAN}░██ ░████ ░██ ░██    ░██ ░██    ░██ ░██        ░██    ░██ ░███     ░██    ░██ ${RESET}"
	@echo "${CYAN}░██░██ ░██░██ ░█████████ ░██    ░██  ░███████  ░█████████ ░██       ░██  ░██  ${RESET}"
	@echo "${CYAN}░████   ░████ ░██        ░███   ░██        ░██ ░██        ░██        ░██░██   ${RESET}"
	@echo "${CYAN}░███     ░███  ░███████  ░██░█████   ░███████   ░███████  ░██         ░███    ${RESET}"
	@echo "${CYAN}==============================================================================${RESET}"
	@echo "${GREEN}--------- WEBSERVER COMPILED SUCCESSFULLY ---------${RESET}"
	@echo
	@echo "          \033[5;1;33m==============================\033[0m"
	@echo "              RUNNING ・ 2026 ・ BY R/B/Z"
	@echo "          \033[5;1;33m==============================\033[0m"
	@echo

$(OBJ_FOLDER)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -rf $(OBJ_FOLDER)

fclean: clean
	@echo
	@echo "\033[3;33m╔════════════════════════════════════════════╗\033[0m"
	@echo "\033[3;33m║  ⚠️ SYSTEM PURGE INITIATED — FCLEAN MODE ⚠️  ║\033[0m"
	@echo "\033[3;33m╚════════════════════════════════════════════╝\033[0m"
	@echo
	@sleep 0.2
	@echo "\033[5;1;31m           WARNING!!   \033[0m"
	@echo
	@echo "\033[3;31m    THIS ACTION IS IRREVERSABLE... \033[0m"
	@sleep 4.0
	@echo "\033[1;36m╔════════════════════════════════════════════╗\033[0m"
	@echo "\033[1;36m║          CLEANUP SEQUENCE COMPLETE         ║\033[0m"
	@echo "\033[1;36m╚════════════════════════════════════════════╝\033[0m"
	 rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

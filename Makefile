NAME = webserv

CXX = c++

CXXFLAGS = -Wall -Werror -Wextra -std=c++98
# -g -O0 -fsanitize=address
INC = -Iinclude
SRC_DIR = src

SRC = $(SRC_DIR)/main.cpp \
	$(SRC_DIR)/Server.cpp \
	$(SRC_DIR)/RequestHandler.cpp \
	$(SRC_DIR)/Router.cpp \
	$(SRC_DIR)/HttpRequest.cpp \
	$(SRC_DIR)/CgiHandler.cpp \
	$(SRC_DIR)/HttpResponse.cpp \
	$(SRC_DIR)/UploadHandler.cpp \
	$(SRC_DIR)/FileHandler.cpp \
	$(SRC_DIR)/Config.cpp \
	$(SRC_DIR)/ConfigParse.cpp

OBJ_FOLDER = obj_files

OBJ_SRC = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_FOLDER)/%.o, $(SRC))

#color
GREEN = \033[0;32m
CYAN = \033[0;36m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJ_SRC)
	$(CXX) $(CXXFLAGS) $(OBJ_SRC) -o $(NAME)
	@clear || true
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
	@echo "            RUNNING ・ 2026 ・ BY R/B/Z"
	@echo "          \033[5;1;33m==============================\033[0m"
	@echo

$(OBJ_FOLDER)/%.o: $(SRC_DIR)/%.cpp
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
	@sleep 2.5
	@echo "\033[1;36m╔════════════════════════════════════════════╗\033[0m"
	@echo "\033[1;36m║          CLEANUP SEQUENCE COMPLETE         ║\033[0m"
	@echo "\033[1;36m╚════════════════════════════════════════════╝\033[0m"
	 rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

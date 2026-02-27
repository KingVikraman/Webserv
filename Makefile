CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -Wno-switch -std=c++98
INCLUDES = -Iinc

NAME = libwebserv.a

CFILES = $(wildcard src/*.cpp)
OFILES = $(CFILES:src/%.cpp=obj/%.o)

TEST_SRC = tests/test_request_line_partial.cpp
TEST_BIN = tests/test_request_line_partial

all: $(NAME)

$(NAME): $(OFILES)
	ar rcs $(NAME) $(OFILES)

obj/%.o: src/%.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_BIN): $(TEST_SRC) $(OFILES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TEST_SRC) $(OFILES) -o $(TEST_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf obj
	rm -f $(TEST_BIN)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all test clean fclean re

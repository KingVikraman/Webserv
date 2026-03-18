CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -Wno-switch -std=c++98
INCLUDES = -Iinc

NAME = libwebserv.a

CFILES = $(wildcard src/*.cpp)
OFILES = $(CFILES:src/%.cpp=obj/%.o)

PARSER_TEST_SRC = tests/test_request_line_partial.cpp
PARSER_TEST_BIN = tests/test_request_line_partial
RESPONSE_TEST_SRC = tests/test_response.cpp
RESPONSE_TEST_BIN = tests/test_response
FILEHANDLER_TEST_SRC = tests/test_file_handler.cpp
FILEHANDLER_TEST_BIN = tests/test_file_handler
MIMETYPE_TEST_SRC = tests/test_mimetype.cpp
MIMETYPE_TEST_BIN = tests/test_mimetype
INTEGRATION_TEST_SRC = tests/test_request_handler.cpp
INTEGRATION_TEST_BIN = tests/test_request_handler

all: $(NAME)

$(NAME): $(OFILES)
	ar rcs $(NAME) $(OFILES)

obj/%.o: src/%.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(PARSER_TEST_BIN): $(PARSER_TEST_SRC) $(OFILES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(PARSER_TEST_SRC) $(OFILES) -o $(PARSER_TEST_BIN)

$(RESPONSE_TEST_BIN): $(RESPONSE_TEST_SRC) $(OFILES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(RESPONSE_TEST_SRC) $(OFILES) -o $(RESPONSE_TEST_BIN)

$(FILEHANDLER_TEST_BIN): $(FILEHANDLER_TEST_SRC) $(OFILES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(FILEHANDLER_TEST_SRC) $(OFILES) -o $(FILEHANDLER_TEST_BIN)

$(MIMETYPE_TEST_BIN): $(MIMETYPE_TEST_SRC) $(OFILES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(MIMETYPE_TEST_SRC) $(OFILES) -o $(MIMETYPE_TEST_BIN)

$(INTEGRATION_TEST_BIN): $(INTEGRATION_TEST_SRC) $(OFILES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(INTEGRATION_TEST_SRC) $(OFILES) -o $(INTEGRATION_TEST_BIN)

test-parser: $(PARSER_TEST_BIN)
	./$(PARSER_TEST_BIN)

test-response: $(RESPONSE_TEST_BIN)
	./$(RESPONSE_TEST_BIN)

test-filehandler: $(FILEHANDLER_TEST_BIN)
	./$(FILEHANDLER_TEST_BIN)

test-mimetype: $(MIMETYPE_TEST_BIN)
	./$(MIMETYPE_TEST_BIN)

test-integration: $(INTEGRATION_TEST_BIN)
	./$(INTEGRATION_TEST_BIN)

test: test-parser test-response test-filehandler test-mimetype test-integration

clean:
	rm -rf obj
	rm -f $(PARSER_TEST_BIN) $(RESPONSE_TEST_BIN) $(FILEHANDLER_TEST_BIN) $(MIMETYPE_TEST_BIN) $(INTEGRATION_TEST_BIN)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all test clean fclean re

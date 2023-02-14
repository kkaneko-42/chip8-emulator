CXX			:= c++
CXXFLAGS	:= -fsanitize=address -fsanitize=undefined -g #-Wall -Wextra -Werror
SRCS_DIR	:= src
TESTS_DIR	:= test

display-test: $(SRCS_DIR)/TerminalDisplay.cpp $(TESTS_DIR)/Display_test.cpp
	$(CXX) $(CXXFLAGS) -D TEST $^ -lncurses -o $@ && ./$@

cpu-test: $(SRCS_DIR)/Cpu.cpp $(TESTS_DIR)/Cpu_test.cpp
	$(CXX) $(CXXFLAGS) -D TEST $^ -o $@ && ./$@

fclean:
	rm -f cpu-test

.PHONY: fclean

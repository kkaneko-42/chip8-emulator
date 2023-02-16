CXX			:= c++
CXXFLAGS	:= -fsanitize=address -fsanitize=undefined -g #-Wall -Wextra -Werror
SRCS_DIR	:= src
TESTS_DIR	:= test
VPATH		:= $(SRCS_DIR):$(TESTS_DIR)

test-all: display-test cpu-test timer-test keyboard-test integration-test

display-test: TerminalDisplay.cpp Display_test.cpp
	$(CXX) $(CXXFLAGS) -D TEST $^ -lncurses -o $@ && ./$@

cpu-test: Cpu.cpp Cpu_test.cpp
	$(CXX) $(CXXFLAGS) -D TEST $^ -o $@ && ./$@

timer-test: $(SRCS_DIR)/Cpu.cpp $(TESTS_DIR)/Cpu_test.cpp
	$(CXX) -pthread -D TEST $^ -o $@ && ./$@

keyboard-test: Keyboard.cpp Keyboard_test.cpp
	$(CXX) $(CXXFLAGS) -D TEST $^ -lncurses -o $@ && ./$@

integration-test: Cpu.cpp Ram.cpp TerminalDisplay.cpp IntegrationTester.cpp
	$(CXX) $(CXXFLAGS) -D TEST $^ -lncurses -o $@ && ./$@

fclean:
	rm -f cpu-test display-test integration-test timer-test keyboard-test

.PHONY: fclean

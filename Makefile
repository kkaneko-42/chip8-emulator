NAME		:= chip8
CXX			:= c++
CXXFLAGS	:= -fsanitize=address -fsanitize=undefined -g #-Wall -Wextra -Werror
SRCS_DIR	:= src
TESTS_DIR	:= test
VPATH		:= $(SRCS_DIR):$(TESTS_DIR)

$(NAME): main.o Cpu.o Ram.o TerminalDisplay.o Keyboard.o Logger.o
	$(CXX) $(CXXFLAGS) $^ -lncurses -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -lncurses

test: display-test cpu-test keyboard-test integration-test

display-test: TerminalDisplay.o Display_test.o
	$(CXX) $(CXXFLAGS) -D TEST $^ -lncurses -o $@ && ./$@

cpu-test: Cpu.o Cpu_test.o
	$(CXX) $(CXXFLAGS) -D TEST $^ -o $@ && ./$@

keyboard-test: Keyboard.o Keyboard_test.o
	$(CXX) $(CXXFLAGS) -D TEST $^ -lncurses -o $@ && ./$@

integration-test: Cpu.o Ram.o TerminalDisplay.o IntegrationTester.o
	$(CXX) $(CXXFLAGS) -D TEST $^ -lncurses -o $@ && ./$@

clean:
	rm -f *.o */*.o $(NAME) cpu-test display-test integration-test timer-test keyboard-test

.PHONY: clean

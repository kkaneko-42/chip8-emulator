CXX			:= c++
CXXFLAGS	:= #-Wall -Wextra -Werror
SRCS_DIR	:= src
TESTS_DIR	:= test

cpu-test: $(SRCS_DIR)/Cpu.cpp $(TESTS_DIR)/Cpu_test.cpp
	$(CXX) $(CXXFLAGS) -D TEST $^ -o $@ && ./$@

fclean:
	rm -f cpu-test

.PHONY: fclean

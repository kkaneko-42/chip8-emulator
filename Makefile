CXX			:= c++
CXXFLAGS	:= #-Wall -Wextra -Werror
SRCS_DIR	:= src
TESTS_DIR	:= test

cpu-test:
	$(CXX) $(CXXFLAGS) -D TEST $(SRCS_DIR)/Cpu.cpp $(TESTS_DIR)/Cpu_test.cpp -o $@ && ./$@

fclean:
	rm -f cpu-test

.PHONY: fclean

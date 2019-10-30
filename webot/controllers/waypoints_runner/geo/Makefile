.PHONY: test clean

CPP_FLAGS=-std=c++17 -Wall

LIB_DIR=lib
TEST_DIR=test 
INCLUDES=\
    -I$(LIB_DIR)\
    -I$(TEST_DIR)

SRCS=\
    geo.cpp\
    geotests.cpp

OBJS=$(SRCS:.cpp=.o)

test: $(OBJS)
	clang++ $(CPP_FLAGS) -o run_test $(OBJS) catch-main.cpp
	./run_test

%.o : %.cpp
	clang++ $(CPP_FLAGS) -c $< -o $@

clean:
	rm $(OBJS)
	rm run_test
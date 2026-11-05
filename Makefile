CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -g
CPPFLAGS = -Iinclude

DEBUG_FLAGS = -O0 -g3
RELEASE_FLAGS = -O3 -DNDEBUG -march=native

ENGINE_SRC = src/main.cpp src/uci.cpp $(TEST_COMMON_SRC)
TEST_COMMON_SRC = src/movegen.cpp \
                  src/position.cpp \
                  src/board.cpp \
                  src/pieces.cpp \
                  src/attacks.cpp \
                  src/move.cpp \
				  src/search.cpp \
				  src/evaluation.cpp \
				  src/openingbook.cpp
TEST_MOVEGEN_SRC = tests/test_movegen.cpp $(TEST_COMMON_SRC)
TEST_MAKE_MOVE_SRC = tests/test_make_move.cpp $(TEST_COMMON_SRC)
TEST_ATTACKS_SRC = tests/test_attacks.cpp $(TEST_COMMON_SRC)
TEST_SEARCH_SRC = tests/test_search.cpp $(TEST_COMMON_SRC)
TEST_PERFT_SRC = tests/test_perft.cpp src/perft.cpp $(TEST_COMMON_SRC)
TEST_BENCHMARK_SRC = tests/test_benchmark.cpp src/benchmark.cpp $(TEST_COMMON_SRC)
TEST_HASH_SRC = tests/test_hash.cpp $(TEST_COMMON_SRC)
TEST_EVAL_SRC = tests/test_eval.cpp $(TEST_COMMON_SRC)

.PHONY: all test clean

all: chess

chess: $(ENGINE_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(RELEASE_FLAGS) $(ENGINE_SRC) -o chess

test: test_movegen test_make_move test_attacks
	./test_movegen
	./test_make_move
	./test_attacks

test_movegen: $(TEST_MOVEGEN_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TEST_MOVEGEN_SRC) -o test_movegen

test_make_move: $(TEST_MAKE_MOVE_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TEST_MAKE_MOVE_SRC) -o test_make_move

test_attacks: $(TEST_ATTACKS_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TEST_ATTACKS_SRC) -o test_attacks

test_search: $(TEST_SEARCH_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TEST_SEARCH_SRC) -o test_search

test_perft: $(TEST_PERFT_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TEST_PERFT_SRC) -o test_perft

test_benchmark: $(TEST_BENCHMARK_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(RELEASE_FLAGS) $(TEST_BENCHMARK_SRC) -o test_benchmark

test_hash: $(TEST_HASH_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TEST_HASH_SRC) -o test_hash

test_eval: $(TEST_EVAL_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TEST_EVAL_SRC) -o test_eval

clean:
	rm -f chess test_movegen test_make_move test_attacks test_search test_perft test_benchmark
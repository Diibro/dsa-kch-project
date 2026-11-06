# ============================================================
# KCH-PMTS  |  Phase 1 Makefile (Modules 1 & 2)
#   make / make all  -> builds ./kch_phase1 (demo driver)
#   make test        -> builds and runs ./test_phase1
#   make run         -> builds and runs the demo
#   make clean       -> removes binaries and object files
# ============================================================
CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -Iinclude

MODULE_OBJS = build/module1_staff.o build/module2_triage.o

all: kch_phase1

kch_phase1: build/main_phase1.o $(MODULE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test_phase1: build/test_phase1.o $(MODULE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile any src/*.cpp into build/*.o
build/%.o: src/%.cpp include/module1_staff.h include/module2_triage.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/test_phase1.o: tests/test_phase1.cpp include/module1_staff.h include/module2_triage.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: test_phase1
	./test_phase1

run: kch_phase1
	./kch_phase1

clean:
	rm -rf build kch_phase1 test_phase1

.PHONY: all test run clean

.PHONY: generate build-project run

generate:
	mkdir build
	make build-project

build-project:
	cd ./build && cmake .. && make --no-print-directory

run:
	./build/simplifier

run-tests:
	./build/tests/test_parser

valgrind-run:
	valgrind --tool=memcheck ./build/simplifier

check:
	cppcheck --language=c++ ./project/src/*.cpp ./main.cpp

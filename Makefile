.PHONY: generate build-project run

generate:
	mkdir build
	make build-project

build-project:
	cd ./build && cmake .. && make --no-print-directory

run:
	./build/simplifier -r ab -f test.txt

run-tests:
	./build/test/to_regex_test && ./build/test/tset_test && ./build/test/graph_test

valgrind-run:
	valgrind --tool=memcheck ./build/simplifier

check:
	cppcheck --language=c++ ./project/src/*.cpp ./main.cpp

console: 
	./build/console
